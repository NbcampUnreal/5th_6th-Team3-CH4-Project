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
	void PlayingGame ();
	void CountDownTimer ();
	void Ready ();
	void EndRound ();
private:
	FTimerHandle TimeCountHandle;

	uint8 bIsGameStart : 1 = false;
	uint8 bIsGameEnd : 1 = false;
	int32 PlayerCount;
	int32 InPlayerCount;

	int32 minutes = 3;
	int32 seconds = 00;

};
