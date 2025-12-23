// (c) 2024. Team03. All rights reserved.

#include "TTLobbyPlayerController.h"
#include "TTGameModeBase_Lobby.h"
#include "Blueprint/UserWidget.h"
#include "TTGameInstance.h"
#include "../Character/TTPlayerState.h"

#pragma region Game Loop

void ATTLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;

		if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
		{
			ServerRPC_InitPlayerInfo(GI->UserNickname, GI->SelectedCharacterRowName);
		}
	}
}

#pragma endregion

#pragma region Server RPC

void ATTLobbyPlayerController::ServerRPC_StartGame_Implementation()
{
    // 1. 호스트 자신을 Ready 상태로 만듦
    if (ATTPlayerState* HostPS = GetPlayerState<ATTPlayerState>())
    {
        if (!HostPS->bIsReady)
        {
            HostPS->bIsReady = true;
            HostPS->ForceNetUpdate();
            HostPS->OnRep_IsReady();
        }
    }

    // 시작 조건 검사: 모든 플레이어가 Ready 상태인지?
    bool bAllReady = true;
    for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(It->Get()))
        {
            if (ATTPlayerState* PS = PC->GetPlayerState<ATTPlayerState>())
            {
                if (!PS->bIsReady)
                {
                    bAllReady = false;
                    break;
                }
            }
        }
    }
    
    // 호스트는 Ready 없이 Start 가능하지만, 다른 플레이어들이 Ready 안되어있으면 시작 불가 (기획에 따라 다름)
    // 여기서는 "모두가 준비되어야 시작 가능"으로 구현 (호스트 제외? or 호스트도 Ready?)
    // 보통 호스트의 Start 버튼이 Ready 역할을 겸하거나, 호스트는 예외.
    // 기획: 호스트가 Start 누르면 시작. 단, 다른 유저들이 Ready 상태여야 함.
    
    if (!bAllReady)
    {
    if (!bAllReady)
    {
        ServerRPC_SendChatMessage(TEXT("All players must be READY to start."));
        return;
    }
    }

	if (ATTGameModeBase_Lobby* GM = GetWorld()->GetAuthGameMode<ATTGameModeBase_Lobby>())
	{
		GM->StartGame();
	}
}

bool ATTLobbyPlayerController::ServerRPC_StartGame_Validate()
{
	return true;
}

void ATTLobbyPlayerController::ServerRPC_ToggleReady_Implementation()
{
    if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
    {
        PS->bIsReady = !PS->bIsReady;
        
        // Force Update
        PS->ForceNetUpdate();
        PS->ForceNetUpdate();
        PS->OnRep_IsReady(); // Execute OnRep on Server as well
    }
}

bool ATTLobbyPlayerController::ServerRPC_ToggleReady_Validate()
{
    return true;
}

void ATTLobbyPlayerController::ServerRPC_InitPlayerInfo_Implementation(const FString& Nickname, const FName& CharacterRowName)
{
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		PS->UserNickname = Nickname;
		PS->SelectedCharacterRowName = CharacterRowName;
        
        // --- Team Balancing Logic ---
        if (AGameStateBase* GS = GetWorld()->GetGameState())
        {
            int32 RedCount = 0;
            int32 BlueCount = 0;
            
            for (APlayerState* OtherPS : GS->PlayerArray)
            {
                if (OtherPS == PS) continue; // Skip self
                
                if (ATTPlayerState* TTPS = Cast<ATTPlayerState>(OtherPS))
                {
                    if (TTPS->GetTeam() == Teams::Red) RedCount++;
                    else if (TTPS->GetTeam() == Teams::Blue) BlueCount++;
                }
            }
            
            // 팀 균형 로직 (Team Balance Logic): 인원수가 적은 팀으로 배정.
            // 인원수가 같을 경우 레드 팀 우선 배정 (예: 첫 번째 플레이어/호스트 -> 레드).
            
            if (RedCount <= BlueCount)
            {
                PS->SetTeam(Teams::Red);
            }
            else
            {
                PS->SetTeam(Teams::Blue);
            }
        }
        // -----------------------------
        
		PS->ForceNetUpdate();

        // Join Message Broadcast (System)
        FString JoinMsg = FString::Printf(TEXT("%s has joined the lobby."), *Nickname);
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(It->Get()))
            {
                // TeamId 0 for System
                PC->ClientRPC_ReceiveChatMessage(TEXT("System"), JoinMsg, 0);
            }
        }
	}
}

bool ATTLobbyPlayerController::ServerRPC_InitPlayerInfo_Validate(const FString& Nickname, const FName& CharacterRowName)
{
	return true;
}

#pragma endregion

void ATTLobbyPlayerController::ServerRPC_SendChatMessage_Implementation(const FString& Message)
{
	if (Message.IsEmpty()) return;

	// 보낸 사람의 닉네임 가져오기
	FString SenderName = TEXT("Unknown");
	uint8 TeamId = 0; // None

	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		SenderName = PS->UserNickname;
		TeamId = (uint8)PS->GetTeam();
	}

	// 모든 클라이언트에게 메시지 전파
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(It->Get()))
		{
			PC->ClientRPC_ReceiveChatMessage(SenderName, Message, TeamId);
		}
	}
}

bool ATTLobbyPlayerController::ServerRPC_SendChatMessage_Validate(const FString& Message)
{
	return true;
}

void ATTLobbyPlayerController::ClientRPC_ReceiveChatMessage_Implementation(const FString& Nickname, const FString& Message, uint8 TeamId)
{
    // Delegate broadcast
    OnChatMessageReceived.Broadcast(Nickname, Message, TeamId);
}



void ATTLobbyPlayerController::Server_RequestChangeTeam_Implementation ( Teams NewTeam )
{
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		PS->SetTeam ( NewTeam );
	}
}
#pragma region Server RPC Team

#pragma endregion