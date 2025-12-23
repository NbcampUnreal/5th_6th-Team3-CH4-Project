// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTPlayerPortraitWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTTPlayerPortraitWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> PlayerNamet;
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> NotificationText;
};
