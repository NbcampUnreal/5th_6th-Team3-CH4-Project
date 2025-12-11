// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_LobbyEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TTGameInstance.h"

void UUW_LobbyEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Join)
	{
		Btn_Join->OnClicked.AddDynamic(this, &ThisClass::OnJoinClicked);
	}
}

void UUW_LobbyEntry::Setup(const FTTSessionInfo& InInfo)
{
	SessionInfo = InInfo;

	if (TextBlock_HostName)
	{
		// SRS Requirement: Display as "Nickname's Game"
		FString DisplayName = FString::Printf(TEXT("%s's Game"), *SessionInfo.HostName);
		TextBlock_HostName->SetText(FText::FromString(DisplayName));
	}

	if (TextBlock_PlayerCount)
	{
		FString CountStr = FString::Printf(TEXT("%d / %d"), SessionInfo.CurrentPlayers, SessionInfo.MaxPlayers);
		TextBlock_PlayerCount->SetText(FText::FromString(CountStr));
	}

	if (TextBlock_Ping)
	{
		FString PingStr = FString::Printf(TEXT("%d ms"), SessionInfo.Ping);
		TextBlock_Ping->SetText(FText::FromString(PingStr));
	}
}

void UUW_LobbyEntry::OnJoinClicked()
{
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->JoinGameSession(SessionInfo.SessionIndex);
	}
}
