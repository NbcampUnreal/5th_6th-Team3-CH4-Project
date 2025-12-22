// Glass_Base.h

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/ThrowableBase.h"
#include "Glass_Bottle.generated.h"

UCLASS ( Abstract )
class TEAM03_API AGlassBase : public AThrowableBase
{
	GENERATED_BODY ()

protected:
	UPROPERTY ( EditDefaultsOnly , Category = "Glass" )
	USoundBase* BreakSound;

	virtual void OnHit (
		UPrimitiveComponent* HitComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		FVector NormalImpulse ,
		const FHitResult& Hit
	) override;

	virtual void Explode_Implementation () override;
};
