// (c) 2024. Team03. All rights reserved.

#include "UW_Option.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UUW_Option::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &ThisClass::OnClickClose);
	}
	if (Btn_Apply)
	{
		Btn_Apply->OnClicked.AddDynamic(this, &ThisClass::OnClickApply);
	}

	InitSettings();
}

#pragma region Helpers

void UUW_Option::InitSettings()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// 해상도 초기화
	if (Combo_Resolution)
	{
		Combo_Resolution->ClearOptions();
		Combo_Resolution->AddOption(TEXT("1920x1080"));
		Combo_Resolution->AddOption(TEXT("1280x720"));
		
		FIntPoint CurrentRes = Settings->GetScreenResolution();
		FString ResString = FString::Printf(TEXT("%dx%d"), CurrentRes.X, CurrentRes.Y);
		Combo_Resolution->SetSelectedOption(ResString);
	}

	// 윈도우 모드 초기화
	if (Combo_WindowMode)
	{
		Combo_WindowMode->ClearOptions();
		Combo_WindowMode->AddOption(TEXT("Fullscreen"));
		Combo_WindowMode->AddOption(TEXT("Windowed"));
		
		EWindowMode::Type CurrentMode = Settings->GetFullscreenMode();
		if (CurrentMode == EWindowMode::Fullscreen)
			Combo_WindowMode->SetSelectedOption(TEXT("Fullscreen"));
		else
			Combo_WindowMode->SetSelectedOption(TEXT("Windowed"));
	}
}

void UUW_Option::ApplySettings()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// 해상도
	if (Combo_Resolution)
	{
		FString Selected = Combo_Resolution->GetSelectedOption();
		FString Left, Right;
		if (Selected.Split(TEXT("x"), &Left, &Right))
		{
			int32 X = FCString::Atoi(*Left);
			int32 Y = FCString::Atoi(*Right);
			Settings->SetScreenResolution(FIntPoint(X, Y));
		}
	}

	// 윈도우 모드
	if (Combo_WindowMode)
	{
		FString Selected = Combo_WindowMode->GetSelectedOption();
		if (Selected == TEXT("Fullscreen"))
			Settings->SetFullscreenMode(EWindowMode::Fullscreen);
		else
			Settings->SetFullscreenMode(EWindowMode::Windowed);
	}

	Settings->ApplySettings(false);
}

#pragma endregion

#pragma region Callbacks

void UUW_Option::OnClickClose()
{
	RemoveFromParent();
}

void UUW_Option::OnClickApply()
{
	ApplySettings();
}

void UUW_Option::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// 즉시 미리보기를 원하면 로직을 여기로 이동, 보통 적용 버튼이 더 좋음
}

void UUW_Option::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
}

void UUW_Option::OnMasterVolumeChanged(float Value)
{
	if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
	{
		GI->SetMasterVolume(Value);
	}
}

#pragma endregion
