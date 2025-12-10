// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTGameModeBase_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTGameModeBase_Lobby : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATTGameModeBase_Lobby();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void StartGame();
};
