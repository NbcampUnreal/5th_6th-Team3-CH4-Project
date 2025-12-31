// (c) 2024. Team03. All rights reserved.

#include "TTGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/CoreOnline.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

UTTGameInstance::UTTGameInstance()
{
	UserNickname = TEXT("Player");
	SelectedCharacterRowName = NAME_None;
    bUseLAN = false; // Default to Steam
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
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnDestroySessionComplete_DelayedCreate));
			
            UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] Cleaning up any previous sessions before creating..."));
            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("[System] Cleaning up existing sessions..."));

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
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (!OnlineSub) return;
    IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("[System] Session Cleanup Done. Creating New Session..."));

    // === Real Create Logic ===
    OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnCreateSessionComplete));

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = bUseLAN; // Saved value
    SessionSettings.NumPublicConnections = 4;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = !bUseLAN;
    SessionSettings.bUseLobbiesIfAvailable = true; // [Fix] Force Lobbies for AppID 480
    
    // 클라이언트에게 알릴 호스트 이름 저장
    FString HostName = UserNickname.IsEmpty() ? TEXT("Unknown") : UserNickname;
    SessionSettings.Set(FName("HostName"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    // [Filter] ViaOnlineService으로 변경 (안정성)
    SessionSettings.Set(FName("PROJECT_ID"), FString("Team03_Project"), EOnlineDataAdvertisementType::ViaOnlineService);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    
    // [Debug] Step-by-step Diagnostics
    if (GEngine)
    {
        // 1. Check Subsystem
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("[System] Subsystem: %s"), *OnlineSub->GetSubsystemName().ToString()));
    
        // 3. Check Local Player
        if (!LocalPlayer)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("[System] FATAL: LocalPlayer is NULL!"));
            return; // Stop execution
        }
        else
        {
            // 4. Check UniqueNetId
            FUniqueNetIdRepl NetId = LocalPlayer->GetPreferredUniqueNetId();
            if (!NetId.IsValid())
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("[System] FATAL: UniqueNetId is INVALID! (Steam Login Failed?)"));
                return; // Stop execution (cannot create session without valid ID)
            }
            else
            {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("[System] UniqueNetId is VALID"));
            }
        }
    }

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("[System] Calling SessionInterface->CreateSession..."));

    bool bResult = SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
    
    if (!bResult)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("[System] CreateSession FAILED immediately (returned false)! Check UniqueNetId or Subsystem state."));
        
        UE_LOG(LogTemp, Error, TEXT("[TTGameInstance] CreateSession returned false."));
        OnCreateSessionCompleteBP.Broadcast(false);
    }
}

void UTTGameInstance::FindGameSessions(bool bIsLAN)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 100; // 검색 범위 확장
            SessionSearch->PingBucketSize = 50; // 핑 50ms 단위로 그룹화 (검색 속도 향상 도움)

            // [Filter] LAN이 아닐 때(Steam)는 "PROJECT_ID"가 일치하는 방만 검색
            if (!bIsLAN)
            {
                SessionSearch->QuerySettings.Set(FName("PROJECT_ID"), FString("Team03_Project"), EOnlineComparisonOp::Equals);
            }

			OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnFindSessionsComplete));

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
		}
	}
}

void UTTGameInstance::JoinGameSession(int32 SessionIndex)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
				FTTSessionInfo Info;
				Info.SessionIndex = i; // 원래 인덱스 저장
				
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

				// Null Subsystem(LAN/에디터)에서는 Host가 세션 생성 시 Open Connection을 소모하지 않음
				// 따라서 0명으로 표시되는 문제를 해결하기 위해 Host(+1)를 추가
				if (IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get())
				{
					if (OnlineSub->GetSubsystemName() == TEXT("NULL"))
					{
						Info.CurrentPlayers++;
					}
				}

				Info.MaxPlayers = MaxConnections;
				Info.Ping = SearchResult.PingInMs;

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
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
		// C++에서 바로 이동하지 않고 UI가 애니메이션 후 이동하도록 변경
		// UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyLevel"), true, TEXT("listen"));
        
    	// 델리게이트 브로드캐스트
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
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}

	OnFindSessionsCompleteBP.Broadcast(bWasSuccessful);
}

void UTTGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectInfo;
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
		{
            // Null Subsystem의 Port 0 문제 수정
            if (ConnectInfo.EndsWith(TEXT(":0")))
            {
                ConnectInfo = ConnectInfo.LeftChop(2); // Remove :0
                ConnectInfo.Append(TEXT(":7777"));
            }

            // 즉시 이동하지 않고 저장 후 UI에 알림
            PendingConnectString = ConnectInfo;
            OnJoinSessionCompleteBP.Broadcast(true);

			// APlayerController* PlayerController = GetFirstLocalPlayerController();
			// if (PlayerController)
			// {
			// 	PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
			// }
		}
	}
    else
    {
        OnJoinSessionCompleteBP.Broadcast(false);
    }
}

void UTTGameInstance::OnDestroySessionBeforeJoin(FName SessionName, bool bWasSuccessful)
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
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
        if (APlayerController* PlayerController = GetFirstLocalPlayerController())
        {
            PlayerController->ClientTravel(PendingConnectString, ETravelType::TRAVEL_Absolute);
        }
    }
}

#pragma endregion
