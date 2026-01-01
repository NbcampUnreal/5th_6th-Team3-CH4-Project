// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTShield.generated.h"

class ATTPlayerCharacter;
class UTTPickupComponent;

UCLASS ()
class TEAM03_API ATTShield : public AActor
{
	GENERATED_BODY ()

public:
	ATTShield ();

	UPROPERTY ( EditAnywhere, Category="Weapon")
	FName WeaponRowName;

	UTTPickupComponent* GetPickupComponent () const { return PickupComponent; }

	UFUNCTION ()
	void HandleOnThrowAway ();
	UFUNCTION ( NetMulticast , Reliable )
	void MulticastThrowAway ();

protected:
	virtual void BeginPlay () override;

	UFUNCTION ()
	void HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter );
	UFUNCTION ()
	void EnablePickupCollision ();

protected:
	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly )
	TObjectPtr<UTTPickupComponent> PickupComponent;
	FTimerHandle CollisionRecoveryTimerHandle;


};
