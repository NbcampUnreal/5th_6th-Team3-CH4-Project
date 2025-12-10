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
	UWorld* World = GetWorld();
	if (World)
	{
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

