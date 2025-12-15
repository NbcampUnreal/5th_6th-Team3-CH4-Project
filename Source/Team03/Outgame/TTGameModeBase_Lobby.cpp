// Fill out your copyright notice in the Description page of Project Settings.


#include "TTGameModeBase_Lobby.h"
#include "GameMapsSettings.h"

ATTGameModeBase_Lobby::ATTGameModeBase_Lobby()
{
	bUseSeamlessTravel = true;
}

void ATTGameModeBase_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ATTGameModeBase_Lobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void ATTGameModeBase_Lobby::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[LOBBY-GM] StartGame Called"));
	
	UWorld* World = GetWorld();
	if (World)
	{
		// Log PlayerState before travel
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
				{
					UE_LOG(LogTemp, Warning, TEXT("[LOBBY-GM] PlayerState Addr: %p, Class: %s"), 
						PS, *PS->GetClass()->GetName());
					
					if (ATTPlayerState* TTPS = Cast<ATTPlayerState>(PS))
					{
						UE_LOG(LogTemp, Warning, TEXT("[LOBBY-GM] Before Travel - PersistedHeadMesh: %s"), 
							TTPS->PersistedHeadMesh ? *TTPS->PersistedHeadMesh->GetName() : TEXT("NULL"));
						UE_LOG(LogTemp, Warning, TEXT("[LOBBY-GM] Before Travel - PersistedBodyMesh: %s"), 
							TTPS->PersistedBodyMesh ? *TTPS->PersistedBodyMesh->GetName() : TEXT("NULL"));
					}
				}
			}
		}
		
		// Get the configured Transition Map from Project Settings
		FString TransitionMapPath = UGameMapsSettings::GetGameMapsSettings()->TransitionMap.GetLongPackageName();
		
		if (TransitionMapPath.IsEmpty())
		{
			// Fallback if not set (though user said it is set to TestMap)
			TransitionMapPath = TEXT("/Game/Maps/TestMap"); 
		}

		FString Url = FString::Printf(TEXT("%s?listen"), *TransitionMapPath);
		World->ServerTravel(Url);
	}
}

