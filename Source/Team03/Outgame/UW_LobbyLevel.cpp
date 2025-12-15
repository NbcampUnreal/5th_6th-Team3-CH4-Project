// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_LobbyLevel.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "TTLobbyPlayerController.h"
#include "TTGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "../Character/TTPlayerState.h"

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

	if (Btn_Customize)
	{
		Btn_Customize->OnClicked.AddDynamic(this, &ThisClass::OnClickCustomize);
	}

	// Update Player List periodically
	GetWorld()->GetTimerManager().SetTimer(PlayerListTimerHandle, this, &ThisClass::UpdatePlayerList, 1.0f, true);
	UpdatePlayerList();
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

void UUW_LobbyLevel::OnClickCustomize()
{
	if (CustomizeWidgetClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), CustomizeWidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
            
             // Set Input Mode Game And UI
            if (APlayerController* PC = GetOwningPlayer())
            {
                 FInputModeGameAndUI InputMode;
                 InputMode.SetWidgetToFocus(Widget->TakeWidget());
                 InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                 PC->SetInputMode(InputMode);
                 PC->SetShowMouseCursor(true);
            }
		}
	}
}

void UUW_LobbyLevel::UpdatePlayerList()
{
	if (!ScrollBox_PlayerList)
	{
		// UE_LOG(LogTemp, Warning, TEXT("[UW_LobbyLevel] ScrollBox_PlayerList is NULL"));
		return;
	}

	ScrollBox_PlayerList->ClearChildren();

	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		// UE_LOG(LogTemp, Log, TEXT("[UW_LobbyLevel] PlayerArray Count: %d"), GS->PlayerArray.Num());

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATTPlayerState* TTPS = Cast<ATTPlayerState>(PS))
			{
				FString DisplayName = TTPS->UserNickname;
				// UE_LOG(LogTemp, Log, TEXT("[UW_LobbyLevel] Found TTPlayerState. Nickname: %s"), *DisplayName);

				if (DisplayName.IsEmpty()) DisplayName = TEXT("Loading...");

				UTextBlock* TextBlock = NewObject<UTextBlock>(this);
				TextBlock->SetText(FText::FromString(DisplayName));
				
				// Background is Black (as per screenshot), so Text should be White or Bright
				TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
                
                // Ensure Font is valid
                FSlateFontInfo FontInfo = TextBlock->GetFont();
                FontInfo.Size = 24.0f; 
                // FontInfo.TypefaceFontName = FName("Bold"); // Optional
                TextBlock->SetFont(FontInfo);

				ScrollBox_PlayerList->AddChild(TextBlock);
				// UE_LOG(LogTemp, Log, TEXT("[UW_LobbyLevel] Added TextBlock for %s. Color: White, Size: 24"), *DisplayName);
			}
			else
			{
				// UE_LOG(LogTemp, Warning, TEXT("[UW_LobbyLevel] PlayerState is NOT ATTPlayerState"));
			}
		}
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("[UW_LobbyLevel] GameState is NULL"));
	}
}

