// ThrowableBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableBase.generated.h"

class UTTPickupComponent;
class ATTPlayerCharacter;

UCLASS ()
class TEAM03_API AThrowableBase : public AActor
{
	GENERATED_BODY ()

public:
	AThrowableBase ();

protected:

	virtual void BeginPlay () override;

	UPROPERTY ( VisibleAnywhere , Category = "Component" )
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "Pickup" )
	TObjectPtr<UTTPickupComponent> PickupComponent;

	UPROPERTY ( VisibleInstanceOnly )
	TObjectPtr<ATTPlayerCharacter> OwnerCharacter;

	UPROPERTY ( Replicated )
	bool bDestroyed = false;

	virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION ()
	virtual void OnHit (
		UPrimitiveComponent* HitComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		FVector NormalImpulse ,
		const FHitResult& Hit
	);

	UFUNCTION ()
	virtual void HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter );

public:

	UFUNCTION ( Server , Reliable )
	void ServerThrow ( const FVector& Direction , float Power );

	virtual void Throw ( const FVector& Direction , float Power );

	UFUNCTION ( BlueprintNativeEvent , Category = "Throwable" )
	void Explode ();
	virtual void Explode_Implementation ();

	UFUNCTION ( BlueprintNativeEvent , Category = "Throwable" )
	void Destruct ();
	virtual void Destruct_Implementation ();
};
