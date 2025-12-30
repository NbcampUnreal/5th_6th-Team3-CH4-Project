// (c) 2024. Team03. All rights reserved.

#include "UW_TitleLevel.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TTGameInstance.h"
#include "UW_LobbyEntry.h"
#include "Animation/WidgetAnimation.h"

void UUW_TitleLevel::NativeConstruct()
{
	Super::NativeConstruct();

	// 이벤트 바인딩
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->PlayBGM(TitleBGM);
	}
    // 애니메이션 바인딩 확인 (디버깅 완료)

	if (Btn_Create)
	{
		Btn_Create->OnClicked.AddDynamic(this, &UUW_TitleLevel::OnCreateClicked);
	}

	if (Btn_Find)
	{
		Btn_Find->OnClicked.AddDynamic(this, &UUW_TitleLevel::OnFindClicked);
	}

	if (Btn_Exit)
	{
		Btn_Exit->OnClicked.AddDynamic(this, &UUW_TitleLevel::OnExitClicked);
	}

	if (Btn_Option)
	{
		Btn_Option->OnClicked.AddDynamic(this, &ThisClass::OnOptionClicked);
	}

	if (Input_Nickname)
	{
		Input_Nickname->OnTextChanged.AddDynamic(this, &UUW_TitleLevel::OnNicknameChanged);
		
		// GameInstance에서 닉네임 초기화
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
		{
			Input_Nickname->SetText(FText::FromString(GI->UserNickname));
		}
	}

	// GameInstance 델리게이트 바인딩
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->OnFindSessionsCompleteBP.AddDynamic(this, &UUW_TitleLevel::OnSessionSearchCompleted);
		GI->OnCreateSessionCompleteBP.AddDynamic(this, &UUW_TitleLevel::OnSessionCreated);
        GI->OnJoinSessionCompleteBP.AddDynamic(this, &ThisClass::OnSessionJoined);
	}

	if (Btn_CloseOverlay)
	{
		Btn_CloseOverlay->OnClicked.AddDynamic(this, &ThisClass::OnCloseOverlayClicked);
	}

	// 입력 모드 설정
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
    
    if (Widget_SessionOverlay)
    {
        Widget_SessionOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }
    
    // 타이틀 진입 시 페이드 인
    if (Anim_FadeIn)
    {
        PlayAnimation(Anim_FadeIn);
    }

	SetLoadingState(false);
}

void UUW_TitleLevel::NativeDestruct()
{
	Super::NativeDestruct();

	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->OnFindSessionsCompleteBP.RemoveDynamic(this, &UUW_TitleLevel::OnSessionSearchCompleted);
	}
}

#pragma region Callbacks

void UUW_TitleLevel::OnCreateClicked()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	SetLoadingState(true);
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		// LAN 연결 vs Steam (GI 설정을 따름)
		GI->CreateGameSession(GI->bUseLAN);
	}
}

void UUW_TitleLevel::OnSessionCreated(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (Anim_FadeOut)
		{
			PlayAnimation(Anim_FadeOut);
            
            // 애니메이션 길이만큼 대기 후 이동 (Timer 사용)
            FTimerHandle WaitHandle;
            float AnimDuration = Anim_FadeOut->GetEndTime() - Anim_FadeOut->GetStartTime();
            GetWorld()->GetTimerManager().SetTimer(WaitHandle, [this]()
            {
                if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
                {
                    GI->TravelToLobby();
                }
            }, AnimDuration, false);
		}
		else
		{
            // 애니메이션 없으면 즉시 이동
			if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
			{
				GI->TravelToLobby();
			}
		}
	}
    else
    {
        SetLoadingState(false);
    }
}

void UUW_TitleLevel::OnSessionJoined(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (Anim_FadeOut)
		{
			PlayAnimation(Anim_FadeOut);
            
            FTimerHandle WaitHandle;
            float AnimDuration = Anim_FadeOut->GetEndTime() - Anim_FadeOut->GetStartTime();
            GetWorld()->GetTimerManager().SetTimer(WaitHandle, [this]()
            {
                if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
                {
                    GI->TravelToPendingSession();
                }
            }, AnimDuration, false);
		}
		else
		{
			if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
			{
				GI->TravelToPendingSession();
			}
		}
	}
    else
    {
        SetLoadingState(false);
        // 실패 메시지 표시 등 추가 가능
    }
}

void UUW_TitleLevel::OnFindClicked()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	SetLoadingState(true);
    
    // 오버레이 표시 및 애니메이션 재생
    if (Widget_SessionOverlay)
    {
        Widget_SessionOverlay->SetVisibility(ESlateVisibility::Visible);
        
        if (Anim_FindInfoSlide)
        {
            PlayAnimation(Anim_FindInfoSlide);
        }
    }
    
    // 목록 초기화
    if (ScrollBox_SessionList) ScrollBox_SessionList->ClearChildren();
    if (TextBlock_NoSessions) TextBlock_NoSessions->SetVisibility(ESlateVisibility::Collapsed);

	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->FindGameSessions(GI->bUseLAN);
	}
}

void UUW_TitleLevel::OnCloseOverlayClicked()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

    if (Widget_SessionOverlay)
    	{
        Widget_SessionOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UUW_TitleLevel::OnOptionClicked()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	if (OptionWidgetClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), OptionWidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
            
            // 포커스 설정
            if (APlayerController* PC = GetOwningPlayer())
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(Widget->TakeWidget());
                PC->SetInputMode(InputMode);
            }
		}
	}
}

void UUW_TitleLevel::OnExitClicked()
{
	if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);

	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UUW_TitleLevel::OnNicknameChanged(const FText& Text)
{
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->UserNickname = Text.ToString();
	}
}

void UUW_TitleLevel::OnSessionSearchCompleted(bool bWasSuccessful)
{
	SetLoadingState(false);

	if (bWasSuccessful)
	{
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
		{
			TArray<FTTSessionInfo> Results = GI->GetSessionSearchResults();
			UpdateSessionList(Results);
            
            // C++ 로직으로 채우기
            if (ScrollBox_SessionList)
            {
                ScrollBox_SessionList->ClearChildren();
                
                if (Results.Num() == 0)
                {
                    if (TextBlock_NoSessions) TextBlock_NoSessions->SetVisibility(ESlateVisibility::Visible);
                }
                else
                {
                    if (TextBlock_NoSessions) TextBlock_NoSessions->SetVisibility(ESlateVisibility::Collapsed);
                    
                    if (LobbyEntryClass)
                    {
                        for (const FTTSessionInfo& Info : Results)
                        {
                            if (UUW_LobbyEntry* EntryWidget = CreateWidget<UUW_LobbyEntry>(this, LobbyEntryClass))
                            {
                                EntryWidget->Setup(Info);
                                ScrollBox_SessionList->AddChild(EntryWidget);
                            }
                        }
                    }
                }
            }
		}
	}
}

#pragma endregion

#pragma region UI State

void UUW_TitleLevel::SetLoadingState(bool bIsLoading)
{
	// 로딩 인디케이터
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(bIsLoading ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

        if (Anim_LoadingShake)
        {
            if (bIsLoading)
            {
                // 무한 반복 (Loop)
                PlayAnimation(Anim_LoadingShake, 0.0f, 0); 
            }
            else
            {
                StopAnimation(Anim_LoadingShake);
            }
        }
	}

	if (Btn_Create) Btn_Create->SetIsEnabled(!bIsLoading);
	if (Btn_Find) Btn_Find->SetIsEnabled(!bIsLoading);
}

#pragma endregion
