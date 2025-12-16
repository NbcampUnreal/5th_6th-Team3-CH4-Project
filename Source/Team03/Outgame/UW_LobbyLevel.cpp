// (c) 2024. Team03. All rights reserved.

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
		
		// 클라이언트인 경우 시작 버튼 숨김
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

	// 플레이어 목록 주기적 업데이트
	GetWorld()->GetTimerManager().SetTimer(PlayerListTimerHandle, this, &ThisClass::UpdatePlayerList, 1.0f, true);
	UpdatePlayerList();
}

#pragma region Callbacks

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
            
             // 게임 및 UI 입력 모드 설정
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
		return;
	}

	ScrollBox_PlayerList->ClearChildren();

	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATTPlayerState* TTPS = Cast<ATTPlayerState>(PS))
			{
				FString DisplayName = TTPS->UserNickname;

				if (DisplayName.IsEmpty()) DisplayName = TEXT("Loading...");

				UTextBlock* TextBlock = NewObject<UTextBlock>(this);
				TextBlock->SetText(FText::FromString(DisplayName));
				
				// 배경이 검은색이므로 텍스트는 흰색 또는 밝은색이어야 함
				TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
                
                // 폰트 유효성 확인
                FSlateFontInfo FontInfo = TextBlock->GetFont();
                FontInfo.Size = 24.0f; 
                // FontInfo.TypefaceFontName = FName("Bold"); // Optional
                TextBlock->SetFont(FontInfo);

				ScrollBox_PlayerList->AddChild(TextBlock);
			}
			else
			{
			}
		}
	}
	else
	{
	}
}

#pragma endregion

