// (c) 2024. Team03. All rights reserved.

#include "TTGameModeBase_Lobby.h"
#include "GameMapsSettings.h"
#include "Team03/Character/TTPlayerState.h"

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
		// Travel 전 PlayerState 로그
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
				{
					if (ATTPlayerState* TTPS = Cast<ATTPlayerState>(PS))
					{
                        // Player State Logic
					}
				}
			}
		}
		
		// 프로젝트 설정에서 설정된 Transition Map 가져오기
		FString TransitionMapPath = UGameMapsSettings::GetGameMapsSettings()->TransitionMap.GetLongPackageName();
		
		if (TransitionMapPath.IsEmpty())
		{
			// 설정되지 않은 경우 대비 (TestMap으로 설정됨)
			TransitionMapPath = TEXT("/Game/Maps/TestMap"); 
		}

		FString Url = FString::Printf(TEXT("%s?listen"), *TransitionMapPath);
		World->ServerTravel(Url);
	}
}

