// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TTAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnCheckHit );
/**
 * 
 */
UCLASS()
class TEAM03_API UTTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UFUNCTION ()
	void AnimNotify_CheckHit ();

public:
	FOnCheckHit OnCheckHit;
};
