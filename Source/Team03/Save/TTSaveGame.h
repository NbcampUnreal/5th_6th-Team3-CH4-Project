// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TTSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTTSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY ( VisibleAnywhere , Category = Basic )
	FSoftObjectPath CurrentHeadMeshPath;

	UPROPERTY ( VisibleAnywhere , Category = Basic )
	FSoftObjectPath CurrentBodyMeshPath;
};
