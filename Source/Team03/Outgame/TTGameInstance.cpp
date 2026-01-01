// (c) 2024. Team03. All rights reserved.

#include "TTGameInstance.h"
#include "Sound/SoundClass.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

UTTGameInstance::UTTGameInstance()
{
	UserNickname = TEXT("Player");
	SelectedCharacterRowName = NAME_None;
    bUseLAN = true; // [Fix] Default to LAN (User Request)
}

void UTTGameInstance::Init()
{
	Super::Init();

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UTTGameInstance::OnNetworkFailure);
	}
	
	// 사운드 믹스 초기화 (볼륨 제어를 위해)
	if (SoundMix_Master)
	{
		UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMix_Master);
	}
}

#pragma region Session Management

void UTTGameInstance::CreateGameSession(bool bIsLAN)
{
    // [Fix] LAN 모드일 때는 'NULL' 서브시스템 강제 사용, Steam일 때는 기본(Steam) 사용
    FName SubsystemName = bIsLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnDestroySessionComplete_DelayedCreate));
			
			
            UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] Cleaning up any previous sessions before creating..."));

			if (!SessionInterface->DestroySession(NAME_GameSession))
            {
                // 즉시 파괴 실패 (존재하지 않음) -> 바로 생성 시도
                SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
                OnDestroySessionComplete_DelayedCreate(NAME_GameSession, true);
            }
		}
	}
}

void UTTGameInstance::OnDestroySessionComplete_DelayedCreate(FName SessionName, bool bWasSuccessful)
{
    // [Fix] 저장된 bUseLAN 상태에 따라 서브시스템 가져오기
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (!OnlineSub) return;
    IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);



    // === Real Create Logic ===
    OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnCreateSessionComplete));

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = bUseLAN; // Saved value
    SessionSettings.NumPublicConnections = 4;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = !bUseLAN;
    
    // [Fix] LAN(NULL Subsystem)은 로비를 지원하지 않음. Steam일 때만 true.
    SessionSettings.bUseLobbiesIfAvailable = !bUseLAN; 
    
    
    // 클라이언트에게 알릴 호스트 이름 저장
    FString HostName = UserNickname.IsEmpty() ? TEXT("Unknown") : UserNickname;
    SessionSettings.Set(FName("HostName"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    // [Filter] 검색(Ping) 단계에서 보이도록 ViaOnlineServiceAndPing으로 복구
    SessionSettings.Set(FName("PROJECT_ID"), FString("Team03_Project"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    
    // [Debug] Step-by-step Diagnostics Removed (User Request)
    if (!LocalPlayer) return;
    if (!LocalPlayer->GetPreferredUniqueNetId().IsValid()) return;

    // [Fix] 가상 LAN (26.xxx) 대응: 로컬 어댑터를 순회하여 가상 IP 탐색 후 강제 방송
    FString VirtualIP;
    ISocketSubsystem* SocketSub = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (SocketSub)
    {
        TArray<TSharedPtr<FInternetAddr>> LocalAddrs;
        if (SocketSub->GetLocalAdapterAddresses(LocalAddrs))
        {
            for (auto& Addr : LocalAddrs)
            {
                FString IPStr = Addr->ToString(false);
                if (IPStr.StartsWith(TEXT("26.")))
                {
                    VirtualIP = IPStr;
                    break;
                }
            }
        }
    }

    if (!VirtualIP.IsEmpty())
    {
        SessionSettings.Set(FName("SERVER_IP_OVERRIDE"), VirtualIP, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
        UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] Virtual LAN IP Detected: %s. Broadcasting override..."), *VirtualIP);
    }

    bool bResult = SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
    
    if (!bResult)
    {
        UE_LOG(LogTemp, Error, TEXT("[TTGameInstance] CreateSession returned false."));
        OnCreateSessionCompleteBP.Broadcast(false);
    }
}

void UTTGameInstance::FindGameSessions(bool bIsLAN)
{
    // [Fix] LAN 모드일 때는 'NULL' 서브시스템 사용
    FName SubsystemName = bIsLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 1000; // [Fix] AppID 480 대응: 대량 검색 후 로컬 필터링
            SessionSearch->PingBucketSize = 50; 

            if (!bIsLAN)
            {
                // Steam 검색 시 Presence 기반 검색 필수
                SessionSearch->QuerySettings.Set(TEXT("SEARCH_PRESENCE"), true, EOnlineComparisonOp::Equals);
            }

			OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnFindSessionsComplete));

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
		}
	}
}

void UTTGameInstance::JoinGameSession(int32 SessionIndex)
{
    // [Fix] 현재 설정된 bUseLAN에 따라 서브시스템 결정
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub && SessionSearch.IsValid())
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid() && SessionSearch->SearchResults.IsValidIndex(SessionIndex))
		{
            // 세션이 이미 존재하는지 확인
            auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
            if (ExistingSession != nullptr)
            {
                PendingJoinSessionIndex = SessionIndex;
                
                OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnDestroySessionBeforeJoin));
                SessionInterface->DestroySession(NAME_GameSession);
                return;
            }

			OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnJoinSessionComplete));

            PendingJoinSessionIndex = SessionIndex; // [Update] Always save for override check
			const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[SessionIndex];

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result))
			{
				// Join Fail logic
			}
		}
	}
}

void UTTGameInstance::DestroyGameSession()
{
    // [Fix] 현재 설정된 bUseLAN에 따라 서브시스템 결정
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnDestroySessionComplete));
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
}

void UTTGameInstance::SetNetworkMode(bool bIsLAN)
{
    bUseLAN = bIsLAN;
}

TArray<FTTSessionInfo> UTTGameInstance::GetSessionSearchResults() const
{
	TArray<FTTSessionInfo> Results;

	if (SessionSearch.IsValid())
	{
		// 자신의 세션을 필터링하기 위한 LocalUserId
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		FUniqueNetIdRepl LocalUserId = LocalPlayer ? LocalPlayer->GetPreferredUniqueNetId() : FUniqueNetIdRepl();

		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); ++i)
		{
			const FOnlineSessionSearchResult& SearchResult = SessionSearch->SearchResults[i];
			
			// 자기 자신의 호스트 필터링
			if (SearchResult.Session.OwningUserId == LocalUserId)
			{
				continue;
			}

			if (SearchResult.IsValid())
			{
                // [Filter] Client-Side Strict Filtering for PROJECT_ID
                FString SessionProjectID;
                if (!SearchResult.Session.SessionSettings.Get(FName("PROJECT_ID"), SessionProjectID) || SessionProjectID != TEXT("Team03_Project"))
                {
                    continue; // 다른 프로젝트의 세션
                }

				FTTSessionInfo Info;
				Info.SessionIndex = i; 
				
				FString HostName;
				if (SearchResult.Session.SessionSettings.Get(FName("HostName"), HostName))
				{
					Info.HostName = HostName;
				}
				else
				{
					Info.HostName = SearchResult.Session.OwningUserName;
				}

				// 현재 플레이어 수 계산
				int32 OpenConnections = SearchResult.Session.NumOpenPublicConnections;
				int32 MaxConnections = SearchResult.Session.SessionSettings.NumPublicConnections;
				
				Info.CurrentPlayers = MaxConnections - OpenConnections;

				// Null Subsystem(LAN) 보정
                IOnlineSubsystem* SearchOnlineSub = IOnlineSubsystem::Get(bUseLAN ? FName("NULL") : NAME_None);
				if (SearchOnlineSub && SearchOnlineSub->GetSubsystemName() == TEXT("NULL"))
				{
                    // Null 서브시스템은 Host 본인을 소모 처리 안 하므로 +1
					Info.CurrentPlayers++;
				}

				Info.MaxPlayers = MaxConnections;
				Info.Ping = SearchResult.PingInMs;

                if (Info.Ping == 9999)
                {
                    // [Debug] LAN 환경에서 9999는 방화벽 차단 가능성 높음
                   // UE_LOG(LogTemp, Warning, TEXT("Session %d has 9999ms ping. Check Firewall/UDP 7777."), i);
                }

				Results.Add(Info);
			}
		}
	}
	return Results;
}

#pragma endregion

#pragma region Session Callbacks

void UTTGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    // [Fix] Callback 시점에서도 올바른 서브시스템 핸들을 해제해야 함
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}
    
    // [Debug] Create Session 결과 로깅
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] Create Session SUCCESS: %s"), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[TTGameInstance] Create Session FAILED: %s"), *SessionName.ToString());
    }

	if (bWasSuccessful)
	{
		// C++에서 바로 이동하지 않고 UI가 애니메이션 후 이동하도록 변경 (User Request)
		// UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyLevel"), true, TEXT("listen"));
        
    	// 델리게이트 브로드캐스트 (UI 연출용)
    	OnCreateSessionCompleteBP.Broadcast(bWasSuccessful);
    }
    else
    {
        // 실패 시에도 UI에 알림
        OnCreateSessionCompleteBP.Broadcast(false);
    }
}

void UTTGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}

	OnFindSessionsCompleteBP.Broadcast(bWasSuccessful);
}

void UTTGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}

	if (Result == EOnJoinSessionCompleteResult::Success && OnlineSub)
	{
		FString ConnectInfo;
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();

        // [Fix] 가상 LAN IP Override 확인
        if (SessionSearch.IsValid() && SessionSearch->SearchResults.IsValidIndex(PendingJoinSessionIndex))
        {
            FString OverriddenIP;
            const FOnlineSessionSearchResult& SearchResult = SessionSearch->SearchResults[PendingJoinSessionIndex];
            if (SearchResult.Session.SessionSettings.Get(FName("SERVER_IP_OVERRIDE"), OverriddenIP))
            {
                ConnectInfo = OverriddenIP; // 포트는 아래 공통 로직에서 붙임
                if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Orange, FString::Printf(TEXT("[System] V-LAN Override Active! Target: %s"), *ConnectInfo));
            }
        }

        // Override가 없으면 표준 방식으로 가져오기
		if (ConnectInfo.IsEmpty())
        {
            SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo);
        }

        // 최종적으로 ConnectInfo가 확보되었는지 확인
        if (!ConnectInfo.IsEmpty())
		{
            // [Fix] LAN(NULL) 환경에서 포트 누락 혹은 0번 포트 대응
            if (OnlineSub->GetSubsystemName() == TEXT("NULL"))
            {
                if (ConnectInfo.EndsWith(TEXT(":0")))
                {
                    ConnectInfo = ConnectInfo.LeftChop(2);
                }
                
                if (!ConnectInfo.Contains(TEXT(":")))
                {
                    ConnectInfo.Append(TEXT(":7777"));
                }
            }

            // [Debug] Join Success -> Check URL
            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Green, FString::Printf(TEXT("[System] Join Success! Ready to travel to: %s"), *ConnectInfo));

            // [Fix] 즉시 이동하지 않고 문자열 저장 (UI 애니메이션 후 BP에서 이동)
            PendingConnectString = ConnectInfo;
            
            // UI 알림 (성공)
            OnJoinSessionCompleteBP.Broadcast(true); 
		}
        else
        {
             if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("[System] Join Success BUT Resolved URL is EMPTY! (Session info invalid?)"));
             OnJoinSessionCompleteBP.Broadcast(false);
        }
	}
    else
    {
        // [Debug] Join Failed Reason
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("[System] Join FAILED! Error Code: %d (0:Success, 1:Full, 2:NoSession, 3:Unknown)"), (int32)Result));
        OnJoinSessionCompleteBP.Broadcast(false);
    }
}

void UTTGameInstance::OnDestroySessionBeforeJoin(FName SessionName, bool bWasSuccessful)
{
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    }

    // 세션 참여 재시도
    if (bWasSuccessful && PendingJoinSessionIndex != -1)
    {
        JoinGameSession(PendingJoinSessionIndex);
        PendingJoinSessionIndex = -1; // 초기화
    }
}

void UTTGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    FName SubsystemName = bUseLAN ? FName("NULL") : NAME_None;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleLevel"));
	}
}

void UTTGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    // 네트워크 실패 처리 (예: 타이틀로 복귀)
}

#pragma endregion

#pragma region Audio System

void UTTGameInstance::SetMasterVolume(float Volume)
{
	if (SoundMix_Master && SoundClass_Master)
	{
		MasterVolume = Volume; // 상태 저장
        
        // [Audio Fix Summary]
        // 엔진 최적화로 인해 Volume이 0.0이 되면 오디오가 Culling(Stop) 되어버림.
        // 다시 볼륨을 올렸을 때 BGM이 처음부터 재생되는 문제가 발생.
        // 해결: 믹스 볼륨을 절대 0.0으로 보내지 않고, 0.001f (Safe Floor)를 유지하여
        // 엔진이 소리를 끄지 않도록 속임. (들리지는 않으나 재생 상태 유지)
        
        float SafeVolume = FMath::Max(Volume, 0.001f); // 0.0001보다 조금 더 높임 (-60dB)
        
        UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix_Master, SoundClass_Master, SafeVolume, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMix_Master);

        if (BGMComponent)
        {
            // 만약 0에서 복구할 때 혹시라도 멈춰있다면 다시 재생 (Restart Fallback)
            if (Volume > 0.01f)
            {
                if (!BGMComponent->IsPlaying())
                {
                    BGMComponent->Play();
                }
            }
        }
	}
	else
	{
		// SoundMix or SoundClass is NULL
	}
}

void UTTGameInstance::PlayBGM(USoundBase* NewBGM)
{
	if (!NewBGM) return;

	// 이미 재생 중인 BGM이 있다면
	if (BGMComponent)
	{
		if (BGMComponent->GetSound() == NewBGM && BGMComponent->IsPlaying())
		{
			return; // 같은 음악이면 계속 재생
		}
		BGMComponent->Stop();
	}
	else
	{
		BGMComponent = UGameplayStatics::CreateSound2D(GetWorld(), NewBGM);
		if (BGMComponent)
		{
			BGMComponent->bIsUISound = true; // UI 사운드로 설정 (일시정지 영향 X 등)
			BGMComponent->bAutoDestroy = false; // 수동 관리
            // BGMComponent->SetVirtualizationMode(EVirtualizationMode::PlayWhenSilent); // ERROR: API 없음
		}
	}

	if (BGMComponent)
	{
		BGMComponent->SetSound(NewBGM); 
		BGMComponent->Play();
	}
}

void UTTGameInstance::StopBGM()
{
	if (BGMComponent)
	{
		BGMComponent->Stop();
	}
}

void UTTGameInstance::PauseBGM(bool bPause)
{
	if (BGMComponent)
	{
		BGMComponent->SetPaused(bPause);
	}
}

void UTTGameInstance::TravelToLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyLevel"), true, TEXT("listen"));
}

void UTTGameInstance::TravelToPendingSession()
{
    if (!PendingConnectString.IsEmpty())
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("[System] Transitioning to Server: %s"), *PendingConnectString));

        if (APlayerController* PlayerController = GetFirstLocalPlayerController())
        {
            PlayerController->ClientTravel(PendingConnectString, ETravelType::TRAVEL_Absolute);
        }
    }
}

#pragma endregion
