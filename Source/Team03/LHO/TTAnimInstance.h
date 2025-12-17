// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TTAnimInstance.generated.h"

class ATTPlayerCharacter;
class UCharacterMovementComponent;

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
	virtual void NativeInitializeAnimation () override;

	virtual void NativeUpdateAnimation (float DeltaSeconds) override;
	FOnCheckHit OnCheckHit;

protected:
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Character" )
	ATTPlayerCharacter* OwnerCharacter;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Character" )
	UCharacterMovementComponent* OwnerCharacterMovement;
	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly )
	uint8 bIsUnarmed : 1;
};
