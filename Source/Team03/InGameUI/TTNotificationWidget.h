// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
#pragma endregion
#pragma region PlayGame
public:
	void CountDownTimer ( int32 minutes , int32 seconds )const;
private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> NotificationText;

	int32 minutes;
	int32 seconds;

#pragma endregion
};
