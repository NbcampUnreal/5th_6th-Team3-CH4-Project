// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTGameInstance.h"
#include "UW_LobbyEntry.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_LobbyEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void Setup(const FTTSessionInfo& InInfo);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_HostName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_PlayerCount;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Ping;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Join;

private:
	UFUNCTION()
	void OnJoinClicked();

	FTTSessionInfo SessionInfo;
};
