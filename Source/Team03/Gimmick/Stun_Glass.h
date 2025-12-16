//Stun_Glass,h

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Glass_Bottle.h"
#include "Stun_Glass.generated.h"

UCLASS ()
class TEAM03_API AStun_Glass : public AGlass_Bottle
{
	GENERATED_BODY ()

public:
	AStun_Glass ();

protected:

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Stun" )
	float StunDamage;

	virtual void Explode_Implementation () override;
};
