// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/ThrowableBase.h"
#include "Glass_Bottle_Object.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API AGlass_Bottle_Object : public AThrowableBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY ( EditDefaultsOnly , Category = "Glass_Obhect" )
	USoundBase* BreakSound;

	virtual void OnHit (
		UPrimitiveComponent* HitComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		FVector NormalImpulse ,
		const FHitResult& Hit
	) override;

};