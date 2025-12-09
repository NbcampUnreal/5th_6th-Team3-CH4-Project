// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TTCharactorHeadSkeletalSelect.generated.h"

/**
 * 
 */
UCLASS(config = DefaultPlayerCharacterHeadSkeletalPaths )
class SELECTSKELETAL_API UTTCharactorHeadSkeletalSelect : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(config)
	TArray<FSoftObjectPath> DefaultPlayerCharacterHeadSkeletalPaths;
};
