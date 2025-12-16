// Stun_Glass.cpp

#include "Stun_Glass.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"


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

	Super::Explode_Implementation ();
}
