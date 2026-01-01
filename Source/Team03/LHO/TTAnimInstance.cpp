// Fill out your copyright notice in the Description page of Project Settings.


#include "TTAnimInstance.h"
#include "Character/TTPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTTAnimInstance::AnimNotify_CheckHit ()
{
	if (OnCheckHit.IsBound () == true)
	{
		OnCheckHit.Broadcast ();
	}

}

void UTTAnimInstance::NativeInitializeAnimation ()
{
	Super::NativeInitializeAnimation ();
	APawn* OwnerPawn = TryGetPawnOwner ();
	if (IsValid ( OwnerPawn ) == true)
	{
		OwnerCharacter = Cast<ATTPlayerCharacter> ( OwnerPawn );
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
	bIsUnarmed = true;
}

void UTTAnimInstance::NativeUpdateAnimation ( float DeltaSeconds )
{
	Super::NativeUpdateAnimation ( DeltaSeconds );
	if (IsValid ( OwnerCharacter ) == true && IsValid ( OwnerCharacterMovement ) == true)
	{
			//bIsUnarmed = OwnerCharacter->GetCurrentWeaponAttackAnimMontage () == nullptr ? true : false;
	}
}