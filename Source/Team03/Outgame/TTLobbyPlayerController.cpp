// Fill out your copyright notice in the Description page of Project Settings.


#include "TTLobbyPlayerController.h"
#include "TTGameModeBase_Lobby.h"
#include "Blueprint/UserWidget.h"

void ATTLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;
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

