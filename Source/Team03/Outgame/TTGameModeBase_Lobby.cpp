// (c) 2024. Team03. All rights reserved.

#include "TTGameModeBase_Lobby.h"
#include "TTLobbyPlayerController.h"
#include "../Character/TTPlayerState.h"
#include "GameMapsSettings.h"
#include "Team03/Character/TTPlayerState.h"
#include "Outgame/TTGameInstance.h"
#include "UW_LobbyLevel.h"
#include "InGameMode/TTGameStateBase.h"


ATTGameModeBase_Lobby::ATTGameModeBase_Lobby()
{
	bUseSeamlessTravel = true;
}

void ATTGameModeBase_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 입장 알림은 InitPlayerInfo에서 처리됨 (닉네임 확정 후 출력)

	if (ATTLobbyPlayerController* LobbyPC = Cast<ATTLobbyPlayerController>(NewPlayer))
	{
        // 첫 번째 플레이어는 호스트로 지정
        if (GetNumPlayers() == 1)
        {
            if (ATTPlayerState* PS = LobbyPC->GetPlayerState<ATTPlayerState>())
            {
                PS->bIsHost = true;
            }
        }
	}
}

void ATTGameModeBase_Lobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ATTPlayerState* PS = Exiting->GetPlayerState<ATTPlayerState>())
	{
		FString LeaveMsg = FString::Printf(TEXT("%s has left the lobby."), *PS->UserNickname);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(It->Get()))
			{
				PC->ClientRPC_ReceiveChatMessage(TEXT("System"), LeaveMsg, 0);
			}
		}
	}
}

void ATTGameModeBase_Lobby::StartGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// 프로젝트 설정에서 설정된 Transition Map 가져오기
		FString TransitionMapPath = UGameMapsSettings::GetGameMapsSettings()->TransitionMap.GetLongPackageName();
		
		if (TransitionMapPath.IsEmpty())
		{
			// 설정되지 않은 경우 대비 (TestMap으로 설정됨)
			TransitionMapPath = TEXT("/Game/Maps/TestMap"); 
		}

		// ----- Ingame 담당자가 추가함 ----- 
		UTTGameInstance* GI = GetGameInstance<UTTGameInstance> ();
		if (IsValid( GI ))
		{
			GI->count = GameState->PlayerArray.Num ();
		}
		// ------------------------------

		FString Url = FString::Printf(TEXT("%s?listen"), *TransitionMapPath);
		World->ServerTravel(Url);
	}
}

