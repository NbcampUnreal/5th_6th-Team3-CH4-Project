// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseGame.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UPauseGame : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct () override;

	UFUNCTION()
	void OnContinueButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked ();
	UFUNCTION()
	void OnOptionButtonClicked ();
	UFUNCTION()
	void OnButtonHovered ();

	void SoundClicked ();

private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UButton> OptionButton;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UButton> ContinueButton;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UButton> ExitButton;

	UPROPERTY ( EditAnywhere )
	TObjectPtr<USoundBase> HoverSound;

	UPROPERTY ( EditAnywhere )
	TObjectPtr<USoundBase> ClickedSound;

	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> Anim_SlideIn;

public:
	UPROPERTY ( VisibleDefaultsOnly )
	TObjectPtr<UUserWidget> OptionWidget;
	UPROPERTY ( EditDefaultsOnly )
	TSubclassOf<UUserWidget> OptionWidgetClass;
};
