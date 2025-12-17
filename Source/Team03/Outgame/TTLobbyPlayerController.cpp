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
	if (ATTGameModeBase_Lobby* GM = GetWorld()->GetAuthGameMode<ATTGameModeBase_Lobby>())
	{
		GM->StartGame();
	}
}

bool ATTLobbyPlayerController::ServerRPC_StartGame_Validate()
{
	return true;
}

void ATTLobbyPlayerController::ServerRPC_InitPlayerInfo_Implementation(const FString& Nickname, const FName& CharacterRowName)
{
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		PS->UserNickname = Nickname;
		PS->SelectedCharacterRowName = CharacterRowName;
		PS->ForceNetUpdate();
	}
}

bool ATTLobbyPlayerController::ServerRPC_InitPlayerInfo_Validate(const FString& Nickname, const FName& CharacterRowName)
{
	return true;
}

#pragma endregion


void ATTLobbyPlayerController::Server_RequestChangeTeam_Implementation ( Teams NewTeam )
{
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		PS->SetTeam ( NewTeam );
	}
}
#pragma region Server RPC Team

#pragma endregion