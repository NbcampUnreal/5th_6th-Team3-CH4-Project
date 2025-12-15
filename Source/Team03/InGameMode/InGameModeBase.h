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
	AInGameModeBase ();
	void SendChatMessage ( const FString& Message );

};
