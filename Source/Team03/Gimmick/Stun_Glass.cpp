//Stun_Glass.cpp

#include "Stun_Glass.h"
#include "Character/TTPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

AStun_Glass::AStun_Glass ()
{
	StunDamage = 30.f;
}

void AStun_Glass::Explode_Implementation ()
{
	if (!HasAuthority ())
	{
		return;
	}

	const FVector Center = GetActorLocation ();

	TArray<AActor*> OverlappedActors;

	UKismetSystemLibrary::SphereOverlapActors (
		GetWorld () ,
		Center ,
		DamageRadius , 
		TArray<TEnumAsByte<EObjectTypeQuery>> () ,
		ATTPlayerCharacter::StaticClass () ,
		TArray<AActor*> () ,
		OverlappedActors
	);

	for (AActor* Actor : OverlappedActors)
	{
		if (ATTPlayerCharacter* Character = Cast<ATTPlayerCharacter> ( Actor ))
		{
			Character->ApplyStun ( StunDamage );
		}
	}

	Super::Explode_Implementation ();

}
