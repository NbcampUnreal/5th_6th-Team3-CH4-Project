// (c) 2024. Team03. All rights reserved.

#include "TTGameModeBase_Lobby.h"
#include "TTLobbyPlayerController.h"
#include "../Character/TTPlayerState.h"
#include "GameMapsSettings.h"
#include "Team03/Character/TTPlayerState.h"
#include "Outgame/TTGameInstance.h"
#include "UW_LobbyLevel.h"
#include "InGameMode/TTGameStateBase.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"


ATTGameModeBase_Lobby::ATTGameModeBase_Lobby()
{
	bUseSeamlessTravel = true;
}

void ATTGameModeBase_Lobby::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // [Critical Check] 패키징 빌드에서 Listen 옵션 누락 시 9999ms 발생
// #if !WITH_EDITOR
    if (!Options.Contains(TEXT("listen")))
    {
        UE_LOG(LogTemp, Error, TEXT("[CRITICAL] Server started WITHOUT '?listen' option! Clients cannot join."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT("[CRITICAL ERROR] Server NOT listening! Check 'OpenLevel' node options for 'listen'."));
        }
    }
    else
    {
        if (GEngine)
        {
            // [Fix] 가상 LAN (26.xxx) 대응: 로컬 어댑터를 순회하여 가상 IP 탐색 후 우선 표시
            FString LocalIP = TEXT("Unknown IP");
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
                            LocalIP = IPStr;
                            break;
                        }
                    }
                    
                    // 가상 IP를 못 찾았다면 첫 번째 주소라도 표시
                    if (LocalIP == TEXT("Unknown IP") && LocalAddrs.Num() > 0)
                    {
                        LocalIP = LocalAddrs[0]->ToString(false);
                    }
                }
            }

            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("[Success] Server is LISTENING on %s:7777"), *LocalIP));
        }
    }
// #endif
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
		// 1. 모든 클라이언트에게 시작 시퀀스(연출) 명령 전달
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(It->Get()))
			{
				PC->ClientRPC_StartGameSequence();
			}
		}

		// 2. 2초 뒤에 실제 이동 (Server Travel)
		World->GetTimerManager().SetTimer(TravelTimerHandle, this, &ThisClass::ProcessServerTravel, 2.0f, false);
	}
}

void ATTGameModeBase_Lobby::ProcessServerTravel()
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
		if (AGameStateBase* GS = World->GetGameState()) // Use Local GS variable/getter
		{
             UTTGameInstance* GI = GetGameInstance<UTTGameInstance>();
             if (IsValid(GI))
             {
                 GI->count = GS->PlayerArray.Num();
             }
		}
		// ------------------------------

		FString Url = FString::Printf(TEXT("%s?listen"), *TransitionMapPath);
		World->ServerTravel(Url);
	}
}

