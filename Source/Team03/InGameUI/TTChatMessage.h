// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTChatMessage.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTTChatMessage : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetChatMessage ( const FString& Message );
private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> ChatTextBox;
};
