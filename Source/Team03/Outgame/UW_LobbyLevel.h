// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyLevel.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_LobbyLevel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StartGame;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Leave;

private:
	UFUNCTION()
	void OnClickStartGame();

	UFUNCTION()
	void OnClickLeave();
};
