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
	virtual void NativeDestruct () override;

	void SetChatMessage ( const FString& Message );

	UFUNCTION()
	void ActivateChat();

	UFUNCTION ( Server , Reliable )
	void ServerRPCPrintString ( const FString& InMessage );
	UFUNCTION ( NetMulticast , Reliable )
	void MulticastRPCPrintString ( const FString& InMessage );
protected:
	UFUNCTION()
	void OnTextCommitted ( const FText& Text , ETextCommit::Type CommitMethod );

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> ChatInputBox;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UScrollBox> ChatScrollBox;
};
