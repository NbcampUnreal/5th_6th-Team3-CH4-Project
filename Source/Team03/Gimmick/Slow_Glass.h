//Slow_Glass.h

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Glass_Bottle.h"
#include "Components/SphereComponent.h"
#include "Slow_Glass.generated.h"

UCLASS ()
class TEAM03_API ASlow_Glass : public AGlassBase
{
	GENERATED_BODY ()

public:
	ASlow_Glass ();

protected:
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Slow" )
	USphereComponent* SlowTrigger;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Slow" )
	float SlowAmount;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Slow" )
	float SlowDuration;

	bool bArmed = false;
	bool bIgnoreInitialOverlap = true;

	virtual void BeginPlay () override;

	UFUNCTION ()
	void OnSlowOverlap (
		UPrimitiveComponent* OverlappedComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		int32 OtherBodyIndex ,
		bool bFromSweep ,
		const FHitResult& SweepResult
	);
};
