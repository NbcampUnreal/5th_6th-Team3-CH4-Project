// (c) 2024. Team03. All rights reserved.

#include "UW_LobbyLevel.h"
#include "UW_Customize.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "TTLobbyPlayerController.h"
#include "TTGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "../Character/TTPlayerState.h"

void UUW_LobbyLevel::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->PlayBGM(LobbyBGM);
	}

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

	if (Btn_RedTeam)
	{
		Btn_RedTeam->OnClicked.AddDynamic(this, &ThisClass::OnClickRedTeam );
	}

	if (Btn_BlueTeam)
	{
		Btn_BlueTeam->OnClicked.AddDynamic(this, &ThisClass::OnClickBlueTeam );
	}
	// 플레이어 목록 주기적 업데이트
	GetWorld()->GetTimerManager().SetTimer(PlayerListTimerHandle, this, &ThisClass::UpdatePlayerList, 1.0f, true);
	UpdatePlayerList();
}

#pragma region Callbacks

void UUW_LobbyLevel::OnClickStartGame()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRPC_StartGame();
	}
}

void UUW_LobbyLevel::OnClickLeave()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->DestroyGameSession();
	}
}

void UUW_LobbyLevel::OnClickCustomize()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

    // 버튼 영역 숨김
    if (VerticalBox_Start)
    {
        VerticalBox_Start->SetVisibility(ESlateVisibility::Hidden);
    }

	if (CustomizeWidgetClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), CustomizeWidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
            
            // 닫힘 이벤트 바인딩
            if (UUW_Customize* CustomWidget = Cast<UUW_Customize>(Widget))
            {
                CustomWidget->OnCustomizeClosed.AddDynamic(this, &ThisClass::OnCustomizeClosedCallback);
            }
            
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

void UUW_LobbyLevel::OnCustomizeClosedCallback()
{
    // 버튼 영역 복구
    if (VerticalBox_Start)
    {
        VerticalBox_Start->SetVisibility(ESlateVisibility::Visible);
    }
}

void UUW_LobbyLevel::OnClickRedTeam ()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController> ( GetOwningPlayer () ))
	{
		PC->Server_RequestChangeTeam(Teams::Red);
	}
}

void UUW_LobbyLevel::OnClickBlueTeam ()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController> ( GetOwningPlayer () ))
	{
		PC->Server_RequestChangeTeam(Teams::Blue );
	}
}

void UUW_LobbyLevel::UpdatePlayerList()
{
	if (!ScrollBox_PlayerListRedTeam || !ScrollBox_PlayerListBlueTeam)
	{
		return;
	}

	ScrollBox_PlayerListRedTeam->ClearChildren();
	ScrollBox_PlayerListBlueTeam->ClearChildren();

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
				if (TTPS->GetTeam() == Teams::Red)
				{
					ScrollBox_PlayerListRedTeam->AddChild(TextBlock);
				}
				if (TTPS->GetTeam() == Teams::Blue)
				{
					ScrollBox_PlayerListBlueTeam->AddChild ( TextBlock );
				}
				else
				{
					TTPS->SetTeam ( Teams::Red );
					ScrollBox_PlayerListRedTeam->AddChild(TextBlock);
				}
				
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

