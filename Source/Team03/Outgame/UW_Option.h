// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Option.generated.h"

class UButton;
class UCheckBox;
class USlider;
class UComboBoxString;
class UWidgetSwitcher;


/**
 * 
 */
UCLASS()
class TEAM03_API UUW_Option : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
#pragma region Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Apply;

	// 비디오 설정
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_Resolution;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_WindowMode;

    // 네트워크 모드
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> Combo_NetworkMode;

#pragma region Graphics Options
	// --- 그래픽 품질 상세 설정 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_TextureQuality;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_ShadowQuality;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_AntiAliasing;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_ViewDistance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> CheckBox_MotionBlur;

	// --- DLSS ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_DLSS;
#pragma endregion

    // 애니메이션 바인딩
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<class UWidgetAnimation> Anim_SlideIn;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ClickSound;
#pragma endregion

private:
#pragma region Widgets // Moved Slider_MasterVolume to private
	// 오디오 설정
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_MasterVolume;
#pragma endregion

#pragma region Callbacks
	UFUNCTION()
	void OnClickClose();

	UFUNCTION()
	void OnClickApply();

	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnNetworkModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// 그래픽 옵션 변경 핸들러
	UFUNCTION()
	void OnWinTextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnShadowQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnMotionBlurChanged(bool bIsChecked);

	UFUNCTION()
	void OnDLSSChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);
#pragma endregion

#pragma region Helpers
	// 헬퍼
	// 헬퍼
	void InitSettings();
	void InitGraphicsSettings(); // 그래픽 설정 초기화 분리
	void ApplySettings();
	void ApplyGraphicsSettings(); // 그래픽 설정 적용 분리
#pragma endregion
};
