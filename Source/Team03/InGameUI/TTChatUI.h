// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTChatUI.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTTChatUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct () override;

	void SetChatMessage ( const FString& Message );

	UFUNCTION()
	void ActivateChat();

protected:
	UFUNCTION()
	void OnTextCommitted ( const FText& Text , ETextCommit::Type CommitMethod );

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> ChatInputBox;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UScrollBox> ChatScrollBox;
};
