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
public:
	void SetNameText (const FString& Name);
	void SetPortraitTexture (UTexture2D* portrait );
private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> PlayerName;
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UImage> playerportrait;
};
