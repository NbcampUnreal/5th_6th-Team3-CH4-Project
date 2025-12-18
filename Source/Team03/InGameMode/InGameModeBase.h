// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API AInGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AInGameModeBase();
	virtual void BeginPlay () override;
	virtual void Tick ( float Delatasecond )override;

	void SendChatMessage ( const FString& Message );

	virtual void PostLogin ( APlayerController* NewPlayer ) override;
	virtual void Logout ( AController* ExitPlayer ) override;

	void StartRound ();
	void EndRound ();
private:
	uint8 bIsGameStart : 1 = false;
	int32 PlayerCount;
};
