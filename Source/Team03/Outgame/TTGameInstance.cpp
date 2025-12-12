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
            // Check if session already exists
            auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
            if (ExistingSession != nullptr)
            {
                // UE_LOG(LogTemp, Warning, TEXT("[TTGameInstance] Session already exists. Destroying before Join..."));
                PendingJoinSessionIndex = SessionIndex;
                
                OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnDestroySessionBeforeJoin));
                SessionInterface->DestroySession(NAME_GameSession);
                return;
            }

			OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UTTGameInstance::OnJoinSessionComplete));

			const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[SessionIndex];
			FString HostName = Result.Session.OwningUserName;
			
			// if (GEngine)
            // {
            //    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Attempting Join -> Host: %s, Index: %d"), *HostName, SessionIndex));
            // }

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result))
            {
				// if (GEngine)
				// {
				//    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("JoinSession Call Failed IMMEDIATELY!"));
				// }
            }
		}
	}
}

void UTTGameInstance::OnDestroySessionBeforeJoin(FName SessionName, bool bWasSuccessful)
{
    // UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] OnDestroySessionBeforeJoin. Success: %d"), bWasSuccessful);

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    }

    // Retry Join
    if (bWasSuccessful && PendingJoinSessionIndex != -1)
    {
        JoinGameSession(PendingJoinSessionIndex);
        PendingJoinSessionIndex = -1; // Reset
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("[TTGameInstance] Failed to destroy session for Join or Invalid Index"));
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
		// Need LocalUserId to filter self
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		FUniqueNetIdRepl LocalUserId = LocalPlayer ? LocalPlayer->GetPreferredUniqueNetId() : FUniqueNetIdRepl();

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
				Info.SessionIndex = i; // Store original index correctly!
				
				FString HostName;
				if (SearchResult.Session.SessionSettings.Get(FName("HostName"), HostName))
				{
					Info.HostName = HostName;
				}
				else
				{
					Info.HostName = SearchResult.Session.OwningUserName;
				}

				// Calculate Current Players
				int32 OpenConnections = SearchResult.Session.NumOpenPublicConnections;
				int32 MaxConnections = SearchResult.Session.SessionSettings.NumPublicConnections;
				
				Info.CurrentPlayers = MaxConnections - OpenConnections;

				Info.MaxPlayers = MaxConnections;
				Info.Ping = SearchResult.PingInMs;

				Results.Add(Info);
			}
		}
	}
	return Results;
}

void UTTGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] OnCreateSessionComplete. Success: %d"), bWasSuccessful);

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
	// UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] OnFindSessionsComplete. Success: %d. Found: %d"), bWasSuccessful, SessionSearch.IsValid() ? SessionSearch->SearchResults.Num() : 0);

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
	// UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] OnJoinSessionComplete. Result: %d"), (int32)Result);

    // if (GEngine)
    // {
    //    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("OnJoinSessionComplete Result: %d"), (int32)Result));
    // }

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
			// UE_LOG(LogTemp, Log, TEXT("[TTGameInstance] Connect String Resolved: %s"), *ConnectInfo);
            
            // Port 0 Fix for Null Subsystem
            if (ConnectInfo.EndsWith(TEXT(":0")))
            {
                // UE_LOG(LogTemp, Warning, TEXT("[TTGameInstance] Port 0 detected, replacing with 7777"));
                ConnectInfo = ConnectInfo.LeftChop(2); // Remove :0
                ConnectInfo.Append(TEXT(":7777"));
            }

            // if (GEngine)
            // {
            //    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Travel URL: %s"), *ConnectInfo));
            // }

			APlayerController* PlayerController = GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("[TTGameInstance] Failed to resolve connect string!"));
            // if (GEngine)
            // {
            //    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Failed to resolve Connect String!"));
            // }
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

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleLevel"));
	}
}

void UTTGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    // Handle network failure (e.g. return to title)
    // SRS doesn't explicitly detail failure recovery but asks for reliability.
}
