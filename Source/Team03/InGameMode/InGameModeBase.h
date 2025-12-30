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
	
//스폰
protected:
	virtual AActor* ChoosePlayerStart_Implementation ( AController* Player ) override;

private:
	int32 BlueSpawnIndex = 0;
	int32 RedSpawnIndex = 0;

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
	void Sendportrait ( const FString& PlayerName , UMaterialInstanceDynamic* portrait ) const;
	void ServerTravelMap ();

	void SetBlueTeamCount ()
	{
		--BlueTeamCount;
	}
	void SetRedTeamCount ()
	{
		--RedTeamCount;
	}
private:
	FTimerHandle TimeCountHandle;
	FTimerHandle EndHandle;


	uint8 bIsGameStart : 1 = false;
	uint8 bIsGameEnd : 1 = false;
	uint8 bTimeUp : 1 = 0;
	int32 PlayerCount;
	int32 InPlayerCount;

	int32 BlueTeamCount;
	int32 RedTeamCount;
	
	int32 minutes = 3;
	int32 seconds = 0;


};
