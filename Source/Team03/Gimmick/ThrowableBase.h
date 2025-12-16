// ThrowableBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableBase.generated.h"

UCLASS ()
class TEAM03_API AThrowableBase : public AActor
{
	GENERATED_BODY ()

public:
	AThrowableBase ();

protected:
	virtual void BeginPlay () override;

	UPROPERTY ( VisibleAnywhere , Category = "Component" )
	UStaticMeshComponent* MeshComp;

	UPROPERTY ( EditAnywhere , Category = "Throwing" )
	float FuseTime;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Throwable" )
	float Damage;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Throwable" )
	float DamageRadius;

	UPROPERTY ( EditDefaultsOnly , Category = "Explosion|Sound" )
	USoundBase* ExplosionSound;

	UPROPERTY ( EditDefaultsOnly , Category = "Explosion|Effect" )
	UParticleSystem* ExplosionParticle;

	FTimerHandle FuseTimerHandle;

	UPROPERTY ( Replicated )
	bool bExploded;

	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

public:

	UFUNCTION ( Server , Reliable )
	void ServerThrow ( const FVector& Direction , float Power );

	virtual void Throw ( const FVector& Direction , float Power );

	UFUNCTION ( BlueprintNativeEvent , Category = "Explosion" )
	void Explode ();
	virtual void Explode_Implementation ();

	UFUNCTION ( NetMulticast , Unreliable )
	void Multicast_ExplodeEffects ();

	UFUNCTION ( BlueprintNativeEvent , Category = "Destruction" )
	void Destruct ();
	virtual void Destruct_Implementation ();
};
