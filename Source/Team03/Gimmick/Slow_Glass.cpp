// Slow_Glass.cpp

#include "Slow_Glass.h"
#include "Character/TTPlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASlow_Glass::ASlow_Glass ()
{
	SlowAmount = 0.5f;
	SlowDuration = 3.f;
}

void ASlow_Glass::Explode_Implementation ()
{
	Super::Explode_Implementation ();

	if (!HasAuthority ()) return;

	TArray<AActor*> Overlaps;
	UGameplayStatics::GetAllActorsOfClass (
		GetWorld () ,
		ATTPlayerCharacter::StaticClass () ,
		Overlaps
	);

	const float Radius = 300.f;

	for (AActor* Actor : Overlaps)
	{
		if (ATTPlayerCharacter* Player = Cast<ATTPlayerCharacter> ( Actor ))
		{
			const float DistSq =
				FVector::DistSquared ( Player->GetActorLocation () , GetActorLocation () );

			if (DistSq <= Radius * Radius)
			{
				Player->ApplySlow ( SlowAmount , SlowDuration );
			}
		}
	}
}
