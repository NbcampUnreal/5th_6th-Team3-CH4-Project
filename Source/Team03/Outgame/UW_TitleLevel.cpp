// Fill out your copyright notice in the Description page of Project Settings.


#include "UW_TitleLevel.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TTGameInstance.h"

void UUW_TitleLevel::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind Events
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

	if (Input_Nickname)
	{
		Input_Nickname->OnTextChanged.AddDynamic(this, &UUW_TitleLevel::OnNicknameChanged);
		
		// Initialize Nickname from GameInstance
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
		{
			Input_Nickname->SetText(FText::FromString(GI->UserNickname));
		}
	}

	// Bind GameInstance Delegate
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->OnFindSessionsCompleteBP.AddDynamic(this, &UUW_TitleLevel::OnSessionSearchCompleted);
	}

	// SRS 4.2: Set Input Mode
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
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

void UUW_TitleLevel::OnCreateClicked()
{
	SetLoadingState(true);
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		// SRS 1.1: LAN Connection
		GI->CreateGameSession(true);
	}
}

void UUW_TitleLevel::OnFindClicked()
{
	SetLoadingState(true);
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->FindGameSessions(true);
	}
}

void UUW_TitleLevel::OnExitClicked()
{
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
		}
	}
}

void UUW_TitleLevel::SetLoadingState(bool bIsLoading)
{
	// SRS 3.9: Loading Indicator
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(bIsLoading ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (Btn_Create) Btn_Create->SetIsEnabled(!bIsLoading);
	if (Btn_Find) Btn_Find->SetIsEnabled(!bIsLoading);
}
