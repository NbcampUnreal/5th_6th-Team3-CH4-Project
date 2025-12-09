// MapsGimmick.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapsGimmick.generated.h"

UCLASS ()
class TEAM03_API AMapsGimmick : public AActor
{
	GENERATED_BODY ()

public:
	AMapsGimmick ();

protected:
	virtual void BeginPlay () override;

	UPROPERTY ( VisibleAnywhere , Category = "Gas Gimmick" )
	class USphereComponent* GasDetectionVolume;

	FTimerHandle GasDamageTimerHandle;

	UPROPERTY ( EditAnywhere , Category = "Gas Stats" )
	float DamageInterval = 1.0f;

	UPROPERTY ( EditAnywhere , Category = "Gas Stats" )
	float DamagePerTick = 5.0f;

	UFUNCTION ()
	void OnOverlapBegin (
		UPrimitiveComponent* OverlappedComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		int32 OtherBodyIndex ,
		bool bFromSweep ,
		const FHitResult& SweepResult );

	UFUNCTION ()
	void OnOverlapEnd (
		UPrimitiveComponent* OverlappedComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		int32 OtherBodyIndex );

	void GasDamage ();

	TArray<ACharacter*> ActorsInGas;

};
