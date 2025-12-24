// Knockback_BOMB.h

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Explosion_Bass.h"
#include "Knockback_BOMB.generated.h"

UCLASS ()
class TEAM03_API AKnockback_BOMB : public AExplosion_Bass
{
	GENERATED_BODY ()

public:
	AKnockback_BOMB ();

protected:
	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "Knockback Bomb" )
	float ExplosionDelay = 2.5f;

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "Knockback Bomb" )
	float KnockbackPower = 1200.f;

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "Knockback Bomb" )
	float StunDamage = 40.f;

	FTimerHandle ExplosionTimerHandle;

	virtual void BeginPlay () override;
	virtual void Explode_Implementation () override;

	void ApplyKnockbackAndStun ();
};
