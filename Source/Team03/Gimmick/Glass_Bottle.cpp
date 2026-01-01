//Glass_Bottle.cpp

#include "Glass_Bottle.h"
#include "Kismet/GameplayStatics.h"

void AGlassBase::OnHit ( UPrimitiveComponent* HitComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , FVector NormalImpulse , const FHitResult& Hit )
{
	if (!HasAuthority ())
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	float HitIntensity = NormalImpulse.Size ();

	if (HitIntensity > 1500.f)
	{
	Explode ();
	}
}

void AGlassBase::Explode_Implementation ()
{
	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation (
			this ,
			BreakSound ,
			GetActorLocation ()
		);
	}

	Super::Explode_Implementation ();

	Destruct ();
}
