// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTSword02.generated.h"

class ATTPlayerCharacter;
class UTTPickupComponent;
class UAnimMontage;
UCLASS()
class TEAM03_API ATTSword02 : public AActor
{
	GENERATED_BODY()
public:

	ATTSword02 ();

	UPROPERTY ( EditAnywhere , Category = "Weapon" )
	FName WeaponRowName;

	UTTPickupComponent* GetPickupComponent () const { return PickupComponent; }
	UAnimMontage* GetAttackMontage () const { return AttackMontage; }

	UFUNCTION ()
	void HandleOnThrowAway ();

protected:
	virtual void BeginPlay () override;

	UFUNCTION ()
	void HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter );

protected:
	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly )
	TObjectPtr<UTTPickupComponent> PickupComponent;

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly )
	TObjectPtr<UAnimMontage> AttackMontage;
};
