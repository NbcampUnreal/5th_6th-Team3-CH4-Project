//Explosion_Bass.h
#pragma once

#include "CoreMinimal.h"
#include "Gimmick/ThrowableBase.h"
#include "Explosion_Bass.generated.h"


UCLASS()
class TEAM03_API AExplosion_Bass : public AThrowableBase
{
	GENERATED_BODY()
	
public:
	AExplosion_Bass();

	UFUNCTION (BlueprintNativeEvent, Category = "Explosion")
	USoundBase* GetExplosionSound() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ExplosionDamage;

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "Explosion" )
	USoundBase* ExplosionSound;

	UFUNCTION (NetMulticast, Reliable)
	void Multicast_ExplosionFX();

	virtual void Explode_Implementation() override;
};
