//Slow_Glass.h

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Glass_Bottle.h"
#include "Slow_Glass.generated.h"

UCLASS ()
class TEAM03_API ASlow_Glass : public AGlass_Bottle
{
	GENERATED_BODY ()

public:
	ASlow_Glass ();

protected:

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Slow" )
	float SlowAmount;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Slow" )
	float SlowDuration;

	virtual void Explode_Implementation () override;
};
