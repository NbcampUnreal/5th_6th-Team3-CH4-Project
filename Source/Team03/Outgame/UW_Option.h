// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Option.generated.h"

class UButton;
class UCheckBox;
class USlider;
class UComboBoxString;

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

	// 오디오 설정
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_MasterVolume;
#pragma endregion

private:
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
	void OnMasterVolumeChanged(float Value);
#pragma endregion

#pragma region Helpers
	// 헬퍼
	void InitSettings();
	void ApplySettings();
#pragma endregion
};
