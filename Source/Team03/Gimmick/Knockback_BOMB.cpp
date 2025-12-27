// Knockback_BOMB.cpp

#include "Knockback_BOMB.h"

#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TTPlayerCharacter.h"

AKnockback_BOMB::AKnockback_BOMB ()
{
	ExplosionDamage = 0.f;
}

void AKnockback_BOMB::BeginPlay ()
{
	Super::BeginPlay ();

	if (HasAuthority ())
	{
		GetWorldTimerManager ().SetTimer (
			ExplosionTimerHandle ,
			this ,
			&AKnockback_BOMB::Explode ,
			ExplosionDelay ,
			false
		);
	}
}

void AKnockback_BOMB::Explode_Implementation ()
{
	if (!HasAuthority () || bDestroyed)
	{
		return;
	}

	bDestroyed = true;

	ApplyKnockbackAndStun ();

	Multicast_ExplosionFX ();

	Destruct ();
}

void AKnockback_BOMB::ApplyKnockbackAndStun ()
{
	TArray<AActor*> Overlaps;

	UGameplayStatics::GetAllActorsOfClass (
		GetWorld () ,
		ATTPlayerCharacter::StaticClass () ,
		Overlaps
	);

	for (AActor* Actor : Overlaps)
	{
		ATTPlayerCharacter* Player = Cast<ATTPlayerCharacter> ( Actor );
		if (!Player)
		{
			continue;
		}

		const float Distance = FVector::Dist (
			GetActorLocation () ,
			Player->GetActorLocation ()
		);

		if (Distance > ExplosionRadius)
		{
			continue;
		}

		FVector KnockbackDir =
			Player->GetActorLocation () - GetActorLocation ();
		KnockbackDir.Normalize ();

		Player->LaunchCharacter (
			KnockbackDir * KnockbackPower ,
			true ,
			true
		);

		Player->ApplyStun ( StunDamage );
	}
}
