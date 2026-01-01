// (c) 2024. Team03. All rights reserved.

#include "UW_LobbyLevel.h"
#include "UW_Customize.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h" 
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "TTLobbyPlayerController.h"
#include "TTGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/TTPlayerState.h"
#include "Engine/World.h"
#include "TimerManager.h"

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
		
		// 클라이언트인 경우 시작 버튼 숨김, Ready 버튼 보임
		if (GetOwningPlayer() && GetOwningPlayer()->GetWorld()->GetNetMode() == NM_Client)
		{
			Btn_StartGame->SetVisibility(ESlateVisibility::Collapsed);
            if (Btn_Ready) Btn_Ready->SetVisibility(ESlateVisibility::Visible);
		}
        else
        {
            // 호스트는 Start 보임, Ready 숨김
             if (Btn_Ready) Btn_Ready->SetVisibility(ESlateVisibility::Collapsed);
        }
	}

    if (Btn_Ready)
    {
        Btn_Ready->OnClicked.AddDynamic(this, &ThisClass::OnClickReady);
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

	// Chat Init
	if (EditableTextBox_ChatInput)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatTextCommitted);
        
        // 2. Hint Text 설정 (반투명 글자)
        EditableTextBox_ChatInput->SetHintText(FText::FromString(TEXT("Chat : ")));
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ATTLobbyPlayerController* LobbyPC = Cast<ATTLobbyPlayerController>(PC))
		{
			LobbyPC->OnChatMessageReceived.AddDynamic(this, &ThisClass::OnChatMessageReceived);
		}
	}
}

void UUW_LobbyLevel::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString Message = Text.ToString();
		if (Message.IsEmpty()) return;
		
		if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(GetOwningPlayer()))
		{
			PC->ServerRPC_SendChatMessage(Message);
		}

		// 입력창 초기화 및 포커스 유지
		EditableTextBox_ChatInput->SetText(FText::GetEmpty());
		EditableTextBox_ChatInput->SetKeyboardFocus();
	}
}

void UUW_LobbyLevel::OnChatMessageReceived(const FString& Nickname, const FString& Message, uint8 TeamId)
{
    // System Message Routing
    if (Nickname == TEXT("System"))
    {
        AddSystemMessage(Message);
        return;
    }

	FString FormattedMsg = FString::Printf(TEXT("%s: %s"), *Nickname, *Message);
	AddChatMessage(FormattedMsg, TeamId);
}

void UUW_LobbyLevel::AddChatMessage(const FString& Message, uint8 TeamId)
{
	if (!ScrollBox_Chat) return;

	UTextBlock* NewText = NewObject<UTextBlock>(ScrollBox_Chat);
	if (NewText)
	{
		NewText->SetText(FText::FromString(Message));
		NewText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 18));
		
        // 1. Team Color Logic
        FLinearColor TextColor = FLinearColor::White;
        if ((Teams)TeamId == Teams::Red) TextColor = FLinearColor::Red;
        else if ((Teams)TeamId == Teams::Blue) TextColor = FLinearColor::Blue; // Standard Blue

		NewText->SetColorAndOpacity(FSlateColor(TextColor));
		NewText->SetAutoWrapText(true);

		ScrollBox_Chat->AddChild(NewText);
		ScrollBox_Chat->ScrollToEnd(); // Auto Scroll
	}
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

void UUW_LobbyLevel::OnClickReady()
{
    if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

    if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(GetOwningPlayer()))
    {
        PC->ServerRPC_ToggleReady();
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
        // Local Player Team 찾기
        Teams LocalPlayerTeam = Teams::None;
        if (APlayerController* PC = GetOwningPlayer())
        {
            if (ATTPlayerState* LocalPS = PC->GetPlayerState<ATTPlayerState>())
            {
                LocalPlayerTeam = LocalPS->GetTeam();
            }
        }
        
        // Input Box Color Update
        if (EditableTextBox_ChatInput)
        {
             FLinearColor TextColor = FLinearColor::White;
             if (LocalPlayerTeam == Teams::Red) TextColor = FLinearColor::Red;
             else if (LocalPlayerTeam == Teams::Blue) TextColor = FLinearColor::Blue;
             
             EditableTextBox_ChatInput->SetForegroundColor(TextColor);
        }

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATTPlayerState* TTPS = Cast<ATTPlayerState>(PS))
			{
                // 팀이 지정되지 않은 경우 목록에 표시하지 않음 (로딩 중 깜빡임 방지)
                if (TTPS->GetTeam() == Teams::None) continue;

				FString DisplayName = TTPS->UserNickname;
                
                if (TTPS->bIsHost)
                {
                    DisplayName.Append(TEXT(" [HOST]"));
                }
                
                if (TTPS->bIsReady)
                {
                    DisplayName.Append(TEXT(" [READY]"));
                }
                
				if (DisplayName.IsEmpty()) DisplayName = TEXT("Loading...");

				UTextBlock* TextBlock = NewObject<UTextBlock>(this);
				TextBlock->SetText(FText::FromString(DisplayName));
				
				// 배경이 검은색이므로 텍스트는 흰색 또는 밝은색이어야 함
                // Player List Team Color
                FLinearColor ListTextColor = FLinearColor::White;
                if (TTPS->GetTeam() == Teams::Red) ListTextColor = FLinearColor::Red;
                else if (TTPS->GetTeam() == Teams::Blue) ListTextColor = FLinearColor::Blue;
                
				TextBlock->SetColorAndOpacity(FSlateColor(ListTextColor));
                
                // 폰트 유효성 확인
                FSlateFontInfo FontInfo = TextBlock->GetFont();
                FontInfo.Size = 24.0f; 
                // FontInfo.TypefaceFontName = FName("Bold"); // Optional
                TextBlock->SetFont(FontInfo);
				if (TTPS->GetTeam() == Teams::Red)
				{
					ScrollBox_PlayerListRedTeam->AddChild(TextBlock);
				}
				else if (TTPS->GetTeam() == Teams::Blue)
				{
					ScrollBox_PlayerListBlueTeam->AddChild ( TextBlock );
				}
                // Else: Do nothing (or handle invalid)
				
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

void UUW_LobbyLevel::AddSystemMessage(const FString& Message)
{
    SystemMessageQueue.Add(Message);
    ProcessSystemMessageQueue();
}

void UUW_LobbyLevel::ProcessSystemMessageQueue()
{
    if (!TextBlock_SystemMessage) return;

	// 현재 메시지가 표시 중인지 확인
	if (bIsShowingSystemMessage)
	{
		float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(SystemMessageTimerHandle);
		float Remaining = GetWorld()->GetTimerManager().GetTimerRemaining(SystemMessageTimerHandle);

		// 스마트 대기열 로직 (Smart Queue Logic):
		// 대기 중인 메시지가 있다면, 현재 메시지의 표시 시간을 1.0초로 단축하여 빠르게 순환시킴.
		if (SystemMessageQueue.Num() > 0)
		{
			if (Elapsed >= 1.0f)
			{
				// 최소 표시 시간이 지났으므로 타이머를 초기화하고 재귀 호출을 통해 즉시 다음 메시지 표시
				GetWorld()->GetTimerManager().ClearTimer(SystemMessageTimerHandle);
				bIsShowingSystemMessage = false;
				// 재귀 호출이 다음 메시지 처리를 담당함
			}
			else
			{
				// 최소 표시 시간이 지나지 않았음; 남은 시간을 조정하여 총 표시 시간이 1.0초가 되도록 설정
				float NewDelay = 1.0f - Elapsed;
				if (NewDelay < Remaining)
				{
					GetWorld()->GetTimerManager().SetTimer(SystemMessageTimerHandle, this, &ThisClass::ProcessSystemMessageQueue, NewDelay, false);
				}
				return; 
			}
		}
		else
		{
			// 대기 중인 메시지 없음. 현재 타이머가 만료되었는지 확인.
			if (Remaining <= 0.0f)
			{
				HideSystemMessage();
			}
			return;
		}
	}

	// 표시 중인 메시지가 없다면, 표시할 대기 메시지가 있는지 확인
	if (SystemMessageQueue.Num() == 0)
	{
		HideSystemMessage();
		return;
	}

	// 큐에서 꺼내와서(Dequeue) 표시
	FString Msg = SystemMessageQueue[0];
	SystemMessageQueue.RemoveAt(0);

	TextBlock_SystemMessage->SetText(FText::FromString(Msg));
	TextBlock_SystemMessage->SetVisibility(ESlateVisibility::Visible);
	bIsShowingSystemMessage = true;

	// 기본 표시 시간: 3.0초
	GetWorld()->GetTimerManager().SetTimer(SystemMessageTimerHandle, this, &ThisClass::ProcessSystemMessageQueue, 3.0f, false);


}

void UUW_LobbyLevel::HideSystemMessage()
{
    if (TextBlock_SystemMessage)
    {
        TextBlock_SystemMessage->SetVisibility(ESlateVisibility::Hidden);
    }
    bIsShowingSystemMessage = false;
}

