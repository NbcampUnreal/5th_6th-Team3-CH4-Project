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
	void PlayStartAnim ();
private:
	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	TObjectPtr<class UWidgetAnimation> StartAnim;
};
