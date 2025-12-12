// Fill out your copyright notice in the Description page of Project Settings.

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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Apply;

	// --- Video Settings ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_Resolution;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> Combo_WindowMode;

	// --- Audio Settings ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_MasterVolume;

private:
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

	// Helpers
	void InitSettings();
	void ApplySettings();
};
