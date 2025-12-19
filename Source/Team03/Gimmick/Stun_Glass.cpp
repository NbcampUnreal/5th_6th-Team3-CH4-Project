//Stun_Glass.cpp

#include "Stun_Glass.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TTPlayerCharacter.h"

AStun_Glass::AStun_Glass ()
{
	StunDamage = 0.f;
}

void AStun_Glass::Explode_Implementation ()
{
	Super::Explode_Implementation ();

	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage (
		this ,
		StunDamage ,
		GetActorLocation () ,
		300.f ,
		nullptr ,
		IgnoreActors ,
		this ,
		GetInstigatorController () ,
		false
	);

}
