// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TTPickupComponent.generated.h"

class ATTPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnPickUp , ATTPlayerCharacter* , InPickUpCharacter );

UCLASS ( Blueprintable , BlueprintType , ClassGroup = (Custom) , meta = (BlueprintSpawnableComponent) )
class TEAM03_API UTTPickupComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:
	UTTPickupComponent ();

protected:
	virtual void BeginPlay () override;

	UFUNCTION ()
	void HandleOnComponentBeginOverlap ( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );

public:
	UPROPERTY ( BlueprintAssignable )
	FOnPickUp OnPickUp;
};
