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
private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UButton> ContinueButton;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UButton> ExitButton;
};
