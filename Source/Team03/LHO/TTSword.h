// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTSword.generated.h"


class ATTPlayerCharacter;
class UTTPickupComponent;

UCLASS()
class TEAM03_API ATTSword : public AActor
{
	GENERATED_BODY()

public:
	ATTSword ();

	UTTPickupComponent* GetPickupComponent () const { return PickupComponent; }

protected:
	virtual void BeginPlay () override;

	UFUNCTION ()
	void HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter );

protected:
	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly )
	TObjectPtr<UTTPickupComponent> PickupComponent;


};
