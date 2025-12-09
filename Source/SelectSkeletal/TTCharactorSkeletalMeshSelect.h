// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TTCharactorSkeletalMeshSelect.generated.h"

/**
 * 
 */
UCLASS(config = DefaultPlayerCharacterSkeletalPaths )
class SELECTSKELETAL_API UTTCharactorSkeletalMeshSelect : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(config)
	TArray<FSoftObjectPath> DefaultPlayerCharacterSkeletalPaths;

};
