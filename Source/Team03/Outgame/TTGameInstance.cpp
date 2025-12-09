// Fill out your copyright notice in the Description page of Project Settings.


#include "TTGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/CoreOnline.h"
#include "Kismet/GameplayStatics.h"

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
}

void UTTGameInstance::CreateGameSession(bool bIsLAN)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// Existing Session Check
			auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if (ExistingSession != nullptr)
			{
				SessionInterface->DestroySession(NAME_GameSession);
			}

			OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnCreateSessionComplete));

			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = bIsLAN;
			SessionSettings.NumPublicConnections = 4; // Default to 4 players
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = true;
			// SessionSettings.bUseLobbiesIfAvailable = true; // Null subsystem might not strictly support Lobbies V2, stick to basic sessions

			// Store Host Name in Session Settings for clients to see
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
			// Null subsystem에서는 presence 검색이 필요하지 않을 수 있음

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
			OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnJoinSessionComplete));

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
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
		int32 Index = 0;
		// Need LocalUserId to filter self
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		FUniqueNetIdRepl LocalUserId = LocalPlayer ? LocalPlayer->GetPreferredUniqueNetId() : FUniqueNetIdRepl();

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			// SRS 2.1: Filter Self-Host
			if (SearchResult.Session.OwningUserId == LocalUserId)
			{
				continue;
			}

			if (SearchResult.IsValid())
			{
				FTTSessionInfo Info;
				Info.SessionIndex = Index;
				
				FString HostName;
				if (SearchResult.Session.SessionSettings.Get(FName("HostName"), HostName))
				{
					Info.HostName = HostName;
				}
				else
				{
					Info.HostName = SearchResult.Session.OwningUserName;
				}

				Info.CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
				Info.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				Info.Ping = SearchResult.PingInMs;

				Results.Add(Info);
			}
			Index++; // Original Index MUST be used for Joining? 
            // Wait, JoinGameSession uses index into SessionSearch->SearchResults.
            // If I filter here for UI logic, the UI index might mismatch SessionSearch Index.
            // CAUTION: passing "Index" which increments every loop is correct if we Join by Index into Results, 
            // but JoinGameSession uses `SessionSearch->SearchResults[SessionIndex]`.
            // So if I skip self, the indices shift.
            // I should store the ORIGINAL index in FTTSessionInfo.
			// Let's fix that.
		}
        
        // RE-LOOP to capture correct index
        Results.Empty();
        for (int32 i = 0; i < SessionSearch->SearchResults.Num(); ++i)
        {
            const FOnlineSessionSearchResult& SearchResult = SessionSearch->SearchResults[i];
            
            // SRS 2.1: Filter Self-Host
            if (SearchResult.Session.OwningUserId == LocalUserId)
            {
                continue;
            }

            if (SearchResult.IsValid())
            {
                FTTSessionInfo Info;
                Info.SessionIndex = i; // Store original index
                
                FString HostName;
                if (SearchResult.Session.SessionSettings.Get(FName("HostName"), HostName))
                {
                    Info.HostName = HostName;
                }
                else
                {
                    Info.HostName = TEXT("Unknown Host");
                }

                Info.CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
                Info.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
                Info.Ping = SearchResult.PingInMs;

                Results.Add(Info);
            }
        }
	}
	return Results;
}

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
        // SRS 2.2 Host Policy says Lobby Level.
        // Assuming we travel to a Lobby map.
        // Needs path to Lobby level.
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
			APlayerController* PlayerController = GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
			}
		}
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
}

void UTTGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    // Handle network failure (e.g. return to title)
    // SRS doesn't explicitly detail failure recovery but asks for reliability.
}
