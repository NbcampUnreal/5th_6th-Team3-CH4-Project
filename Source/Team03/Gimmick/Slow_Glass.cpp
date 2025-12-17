//Slow_Glass.cpp

#include "Slow_Glass.h"

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
	Super::Explode_Implementation ();
}
