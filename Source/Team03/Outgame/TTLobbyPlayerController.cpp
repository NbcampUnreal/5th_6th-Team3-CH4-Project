// Fill out your copyright notice in the Description page of Project Settings.


#include "TTLobbyPlayerController.h"
#include "TTGameModeBase_Lobby.h"
#include "Blueprint/UserWidget.h"
#include "TTGameInstance.h"
#include "../Character/TTPlayerState.h"

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

