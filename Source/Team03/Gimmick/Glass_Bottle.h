//Glass_Bottle.h

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/ThrowableBase.h"
#include "Glass_Bottle.generated.h"

UCLASS()
class TEAM03_API AGlass_Bottle : public AThrowableBase
{
	GENERATED_BODY()

protected:

	UPROPERTY ( EditDefaultsOnly , Category = "Explosion|Sound" )
	USoundBase* BreakSound;

	virtual void Explode_Implementation() override;
};
