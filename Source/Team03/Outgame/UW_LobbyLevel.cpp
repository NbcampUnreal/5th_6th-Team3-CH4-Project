// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_LobbyLevel.h"
#include "Components/Button.h"
#include "TTLobbyPlayerController.h"
#include "TTGameInstance.h"

void UUW_LobbyLevel::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_StartGame)
	{
		Btn_StartGame->OnClicked.AddDynamic(this, &ThisClass::OnClickStartGame);
		
		// Hide Start Button if Client
		if (GetOwningPlayer() && GetOwningPlayer()->GetWorld()->GetNetMode() == NM_Client)
		{
			Btn_StartGame->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (Btn_Leave)
	{
		Btn_Leave->OnClicked.AddDynamic(this, &ThisClass::OnClickLeave);
	}
}

void UUW_LobbyLevel::OnClickStartGame()
{
	if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRPC_StartGame();
	}
}

void UUW_LobbyLevel::OnClickLeave()
{
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->DestroyGameSession();
	}
}

