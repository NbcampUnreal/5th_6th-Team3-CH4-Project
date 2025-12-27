// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "TTNotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTTNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct () override;
#pragma region Start
	void PlayStartAnim ();
private:
	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> StartAnim;

	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> OnAnim;

	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> EndAnim;

	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> LoseAnim;
	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> WinAnim;
	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> DeadAnim;
#pragma endregion
#pragma region PlayGame
public:
	void CountDownTimer ( int32 minutes , int32 seconds )const;
	void OnAnimation ();
	void EndAnimation ();
private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> NotificationText;

public:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UVerticalBox> PlayerportraitBox;

#pragma endregion
};
