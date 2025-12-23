// (c) 2024. Team03. All rights reserved.

#include "UW_Option.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" // Added for GetSupportedFullscreenResolutions
#include "Animation/WidgetAnimation.h"
#include "Components/CheckBox.h"
#include "TTGameInstance.h"
#include "Misc/Optional.h"

// [DLSS Toggle] 팀원 플러그인 미보유 이슈로 비활성화. 활성화하려면 1로 변경.
#define ENABLE_DLSS 0

#if ENABLE_DLSS
// DLSS
#include "DLSSBlueprint/Public/DLSSLibrary.h"
#endif


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

	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->OnValueChanged.AddDynamic(this, &ThisClass::OnMasterVolumeChanged);
        
        // 초기 값 설정 (Store된 값 가져오기)
        if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
        {
             Slider_MasterVolume->SetValue(GI->MasterVolume);
        }
        else
        {
             Slider_MasterVolume->SetValue(1.0f); 
        }
	}
    
	// 등장 애니메이션
    if (Anim_SlideIn)
    {
        PlayAnimation(Anim_SlideIn);
    }

	if (Combo_TextureQuality) Combo_TextureQuality->OnSelectionChanged.AddDynamic(this, &ThisClass::OnWinTextureQualityChanged);
	if (Combo_ShadowQuality) Combo_ShadowQuality->OnSelectionChanged.AddDynamic(this, &ThisClass::OnShadowQualityChanged);
	if (Combo_AntiAliasing) Combo_AntiAliasing->OnSelectionChanged.AddDynamic(this, &ThisClass::OnAntiAliasingChanged);
	if (Combo_ViewDistance) Combo_ViewDistance->OnSelectionChanged.AddDynamic(this, &ThisClass::OnViewDistanceChanged);
	if (CheckBox_MotionBlur) CheckBox_MotionBlur->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnMotionBlurChanged);
#if ENABLE_DLSS
	if (Combo_DLSS) Combo_DLSS->OnSelectionChanged.AddDynamic(this, &ThisClass::OnDLSSChanged);
#endif

	InitSettings();
	InitGraphicsSettings();
}

#pragma region Helpers

void UUW_Option::InitSettings()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// 해상도 초기화는 하단에서 일괄 처리 (데스크탑 해상도 고려)

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

    // 해상도 초기화 (동적 - 하드웨어 지원 목록 사용)
    if (Combo_Resolution)
    {
        Combo_Resolution->ClearOptions();
        
        TArray<FIntPoint> Resolutions;
        bool bResult = UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
        
        if (bResult && Resolutions.Num() > 0)
        {
            // 중복 제거 및 정렬
            Resolutions.Sort([](const FIntPoint& A, const FIntPoint& B){
                return (A.X * A.Y) > (B.X * B.Y); // 픽셀 수 기준 내림차순
            });
            
            FIntPoint LastAdded = FIntPoint::ZeroValue;
            for(const FIntPoint& Res : Resolutions)
            {
                // 너무 작은 해상도 제외
                if (Res.X < 1280) continue;
                
                if (Res != LastAdded)
                {
                    Combo_Resolution->AddOption(FString::Printf(TEXT("%dx%d"), Res.X, Res.Y));
                    LastAdded = Res;
                }
            }
        }
        else
        {
             // 실패 시 데스크탑 해상도 + FHD Fallback
             FIntPoint DesktopRes = Settings->GetDesktopResolution();
             Combo_Resolution->AddOption(FString::Printf(TEXT("%dx%d"), DesktopRes.X, DesktopRes.Y));
             if (DesktopRes != FIntPoint(1920, 1080)) Combo_Resolution->AddOption(TEXT("1920x1080"));
             Combo_Resolution->AddOption(TEXT("1280x720"));
        }

        // 현재 설정값 선택
        FIntPoint CurrentRes = Settings->GetScreenResolution();
        FString ResString = FString::Printf(TEXT("%dx%d"), CurrentRes.X, CurrentRes.Y);
        Combo_Resolution->SetSelectedOption(ResString);
        
        // 목록에 없으면 추가
        if (Combo_Resolution->FindOptionIndex(ResString) == -1)
        {
             Combo_Resolution->AddOption(ResString);
             Combo_Resolution->SetSelectedOption(ResString);
        }
    }
}

void UUW_Option::InitGraphicsSettings()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// Helper Lambda to fill 0~3 options
	auto FillQualityOptions = [](UComboBoxString* Combo, int32 CurrentValue)
	{
		if (!Combo) return;
		Combo->ClearOptions();
		Combo->AddOption(TEXT("Low"));
		Combo->AddOption(TEXT("Medium"));
		Combo->AddOption(TEXT("High"));
		Combo->AddOption(TEXT("Epic"));
		
		CurrentValue = FMath::Clamp(CurrentValue, 0, 3);
		if (CurrentValue == 0) Combo->SetSelectedOption(TEXT("Low"));
		else if (CurrentValue == 1) Combo->SetSelectedOption(TEXT("Medium"));
		else if (CurrentValue == 2) Combo->SetSelectedOption(TEXT("High"));
		else Combo->SetSelectedOption(TEXT("Epic"));
	};

	FillQualityOptions(Combo_TextureQuality, Settings->GetTextureQuality());
	FillQualityOptions(Combo_ShadowQuality, Settings->GetShadowQuality());
	FillQualityOptions(Combo_AntiAliasing, Settings->GetAntiAliasingQuality());
	FillQualityOptions(Combo_ViewDistance, Settings->GetViewDistanceQuality());

	// Motion Blur (Console Variable Check)
	if (CheckBox_MotionBlur)
	{
		static IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
        
        // Force OFF by default if desired (여기서는 유저 요청대로 초기값을 OFF로 강제)
        if (CVar && CVar->GetInt() > 0)
        {
            // 하지만 이미 유저가 켰을 수도 있으므로, 저장된 값이 없을 때만 꺼야 함.
            // 여기서는 단순하게, 위젯 열릴 때 CVar 상태를 반영하되, 
            // 만약 "Default OFF"를 원한다면 초기화 시점에 꺼버릴 수도 있음.
            // 요청사항: "초기값은 OFF여야 함". -> CVar를 0으로 설정.
            
            // 하지만 매번 끄면 옵션을 켜도 다시 들어오면 꺼짐.
            // SaveGame 시스템이 없으므로 GameUserSettings에 의존하거나, 
            // 여기서는 그냥 현재 상태를 보여주는게 맞음.
            // 단, "초기값"이 Off가 아니라는 불만이므로, 최초 실행 시 Off로 설정되도록 해야 함.
            // GameUserSettings에 MotionBlur 항목이 없으므로 수동 관리 필요.
            // 일단 현재 값을 반영하되, 만약 켜져있다면... 사용자가 킨 것일 수도 있음.
            
            // 타협: CVar의 현재 값을 신뢰하되, r.MotionBlurQuality는 엔진 기본값이 4임.
            // 따라서 4라면(기본값) 0으로 강제 변경.
            if (CVar->GetInt() == 4) 
            {
                CVar->Set(0, ECVF_SetByGameSetting);
            }
        }
        
		bool bIsOn = (CVar && CVar->GetInt() > 0);
		CheckBox_MotionBlur->SetIsChecked(bIsOn);
	}

	// DLSS Initialization
#if ENABLE_DLSS
	if (Combo_DLSS)
	{
		Combo_DLSS->ClearOptions();

		// Check DLSS Support
		bool bSupported = UDLSSLibrary::IsDLSSSupported();
		
		if (!bSupported)
		{
			Combo_DLSS->AddOption(TEXT("Disable"));
			Combo_DLSS->SetSelectedOption(TEXT("Disable"));
			Combo_DLSS->SetIsEnabled(false);
		}
		else
		{
			Combo_DLSS->AddOption(TEXT("Off")); // Default
			Combo_DLSS->AddOption(TEXT("Quality"));
			Combo_DLSS->AddOption(TEXT("Balanced"));
			Combo_DLSS->AddOption(TEXT("Performance"));
			Combo_DLSS->SetIsEnabled(true);

			UDLSSMode CurrentMode = UDLSSLibrary::GetDLSSMode();
            
            // 유저 요청: "초기값은 OFF".
            if (CurrentMode != UDLSSMode::Off)
            {
                 UDLSSLibrary::SetDLSSMode(this, UDLSSMode::Off);
                 CurrentMode = UDLSSMode::Off;
            }

			switch (CurrentMode)
			{
			case UDLSSMode::Off: Combo_DLSS->SetSelectedOption(TEXT("Off")); break;
			case UDLSSMode::Quality: Combo_DLSS->SetSelectedOption(TEXT("Quality")); break;
			case UDLSSMode::Balanced: Combo_DLSS->SetSelectedOption(TEXT("Balanced")); break;
			case UDLSSMode::Performance: Combo_DLSS->SetSelectedOption(TEXT("Performance")); break;
			default: Combo_DLSS->SetSelectedOption(TEXT("Off")); break;
			}
		}
	}
#else
    if (Combo_DLSS)
    {
        Combo_DLSS->ClearOptions();
        Combo_DLSS->AddOption(TEXT("Disabled (No Plugin)"));
        Combo_DLSS->SetSelectedOption(TEXT("Disabled (No Plugin)"));
        Combo_DLSS->SetIsEnabled(false);
    }
#endif
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

void UUW_Option::ApplyGraphicsSettings()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// Texture
	if (Combo_TextureQuality)
	{
		int32 Val = Combo_TextureQuality->GetSelectedIndex();
		if (Val >= 0) Settings->SetTextureQuality(Val);
	}
	// Shadow
	if (Combo_ShadowQuality)
	{
		int32 Val = Combo_ShadowQuality->GetSelectedIndex();
		if (Val >= 0) Settings->SetShadowQuality(Val);
	}
	// AA
	if (Combo_AntiAliasing)
	{
		int32 Val = Combo_AntiAliasing->GetSelectedIndex();
		if (Val >= 0) Settings->SetAntiAliasingQuality(Val);
	}
	// ViewDistance
	if (Combo_ViewDistance)
	{
		int32 Val = Combo_ViewDistance->GetSelectedIndex();
		if (Val >= 0) Settings->SetViewDistanceQuality(Val);
	}
	
	// Motion Blur
	if (CheckBox_MotionBlur)
	{
		int32 Quality = CheckBox_MotionBlur->IsChecked() ? 4 : 0;
		IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
		if (CVar) CVar->Set(Quality, ECVF_SetByGameSetting);
	}

	// DLSS
#if ENABLE_DLSS
	if (Combo_DLSS && Combo_DLSS->GetIsEnabled())
	{
		FString Selected = Combo_DLSS->GetSelectedOption();
		UDLSSMode Mode = UDLSSMode::Off;
		
		if (Selected == TEXT("Quality")) Mode = UDLSSMode::Quality; // Fixed
		else if (Selected == TEXT("Balanced")) Mode = UDLSSMode::Balanced;
		else if (Selected == TEXT("Performance")) Mode = UDLSSMode::Performance; // Fixed
		
		UDLSSLibrary::SetDLSSMode(this, Mode); // Fixed: Added 'this'
	}
#endif

	Settings->ApplySettings(false);
}


#pragma endregion

#pragma region Callbacks

void UUW_Option::OnClickClose()
{
    if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);
	RemoveFromParent();
}

void UUW_Option::OnClickApply()
{
    if (ClickSound) UGameplayStatics::PlaySound2D(this, ClickSound);
	ApplySettings();
	ApplyGraphicsSettings();
}

void UUW_Option::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// 필요 시 즉시 적용 로직 추가
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

void UUW_Option::OnWinTextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// Optional: Apply immediately
	// if (GGameUserSettings) { GGameUserSettings->SetTextureQuality(Combo_TextureQuality->GetSelectedIndex()); GGameUserSettings->ApplySettings(false); }
}

void UUW_Option::OnShadowQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
}

void UUW_Option::OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
}

void UUW_Option::OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
}

void UUW_Option::OnMotionBlurChanged(bool bIsChecked)
{
	// Apply immediately for visual feedback
	int32 Quality = bIsChecked ? 4 : 0;
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
	if (CVar) CVar->Set(Quality, ECVF_SetByGameSetting);
}

void UUW_Option::OnDLSSChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
#if ENABLE_DLSS
	if (Combo_DLSS && Combo_DLSS->GetIsEnabled())
	{
		UDLSSMode Mode = UDLSSMode::Off;
		if (SelectedItem == TEXT("Quality")) Mode = UDLSSMode::Quality; // Fixed
		else if (SelectedItem == TEXT("Balanced")) Mode = UDLSSMode::Balanced;
		else if (SelectedItem == TEXT("Performance")) Mode = UDLSSMode::Performance; // Fixed
		
		UDLSSLibrary::SetDLSSMode(this, Mode); // Fixed: Added 'this'
	}
#endif
}

#pragma endregion
