//Slow_Glass.cpp

#include "Slow_Glass.h"
#include "Character/TTPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

ASlow_Glass::ASlow_Glass ()
{
	SlowAmount = 0.5f;
	SlowDuration = 3.f;
}

void ASlow_Glass::Explode_Implementation ()
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
			Character->ApplySlow ( SlowAmount , SlowDuration );
		}
	}

	Super::Explode_Implementation ();
}
