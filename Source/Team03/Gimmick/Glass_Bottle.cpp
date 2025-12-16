//Glass_Bottle.CPP

#include "Glass_Bottle.h"
#include "Kismet/GameplayStatics.h"

void AGlass_Bottle::Explode_Implementation ()
{
	if (!HasAuthority ()) return;

	Super::Explode_Implementation ();
}
