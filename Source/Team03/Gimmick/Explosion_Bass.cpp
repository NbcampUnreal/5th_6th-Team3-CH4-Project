//Explosion_Bass,cpp

#include "Explosion_Bass.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

AExplosion_Bass::AExplosion_Bass()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

USoundBase* AExplosion_Bass::GetExplosionSound_Implementation() const
{
	return ExplosionSound;
}

void AExplosion_Bass::Explode_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	UGameplayStatics::ApplyRadialDamage(
		this ,
		ExplosionDamage ,
		GetActorLocation() ,
		ExplosionRadius ,
		nullptr ,
		TArray<AActor*>() ,
		this ,
		GetInstigatorController() ,
		true
	);
	Multicast_ExplosionFX();

	Destroy();
}

void AExplosion_Bass::Multicast_ExplosionFX_Implementation()
{
	if (USoundBase* Sound = GetExplosionSound())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this ,
			Sound ,
			GetActorLocation()
		);
	}
}