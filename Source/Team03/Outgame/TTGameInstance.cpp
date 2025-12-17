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
			// 기존 세션 확인
			auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if (ExistingSession != nullptr)
			{
				SessionInterface->DestroySession(NAME_GameSession);
			}

			OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnCreateSessionComplete));

			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = bIsLAN;
			SessionSettings.NumPublicConnections = 4; // 최대 4명
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = true;
			
			// 클라이언트에게 알릴 호스트 이름 저장
			FString HostName = UserNickname.IsEmpty() ? TEXT("Unknown") : UserNickname;
			SessionSettings.Set(FName("HostName"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
		}
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
			SessionSearch->MaxSearchResults = 20;

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

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyLevel"), true, TEXT("listen"));
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

			APlayerController* PlayerController = GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
			}
		}
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
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix_Master, SoundClass_Master, Volume, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMix_Master);
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

#pragma endregion
