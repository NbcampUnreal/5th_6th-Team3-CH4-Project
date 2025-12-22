//Slow_Glass.cpp

#include "Slow_Glass.h"
#include "Character/TTPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ASlow_Glass::ASlow_Glass ()
{
	SlowAmount = 0.5f;
	SlowDuration = 3.f;
}

void ASlow_Glass::Explode_Implementation ()
{
	Super::Explode_Implementation ();

	TArray<AActor*> Overlaps;
	UGameplayStatics::GetAllActorsOfClass (
		GetWorld () ,
		ATTPlayerCharacter::StaticClass () ,
		Overlaps
	);

	for (AActor* Actor : Overlaps)
	{
		if (ATTPlayerCharacter* Player = Cast<ATTPlayerCharacter> ( Actor ))
		{

		}
	}
}
