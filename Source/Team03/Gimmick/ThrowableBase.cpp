// ThrowableBase.cpp

#include "ThrowableBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AThrowableBase::AThrowableBase ()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "MeshComp" ) );
	RootComponent = MeshComp;

	MeshComp->SetSimulatePhysics ( true );
	MeshComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
	MeshComp->SetCollisionObjectType ( ECC_PhysicsBody );

	bReplicates = true;

	FuseTime;
	Damage;
	DamageRadius;

	bExploded = false;
}

void AThrowableBase::BeginPlay ()
{
	Super::BeginPlay ();

	if (HasAuthority ())
	{
		GetWorld ()->GetTimerManager ().SetTimer (
			FuseTimerHandle ,
			this ,
			&AThrowableBase::Explode ,
			FuseTime ,
			false
		);
	}
}

void AThrowableBase::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( AThrowableBase , bExploded );
}


void AThrowableBase::ServerThrow_Implementation ( const FVector& Direction , float Power )
{
	Throw ( Direction , Power );
}

void AThrowableBase::Throw ( const FVector& Direction , float Power )
{
	if (!MeshComp) return;

	MeshComp->AddImpulse ( Direction * Power , NAME_None , true );
}

void AThrowableBase::Explode_Implementation ()
{
	if (bExploded) return;

	bExploded = true;

	if (HasAuthority ())
	{
		UGameplayStatics::ApplyRadialDamage (
			this ,
			Damage ,
			GetActorLocation () ,
			DamageRadius ,
			nullptr ,
			TArray<AActor*> () ,
			this ,
			GetInstigatorController () ,
			true
		);

		Multicast_ExplodeEffects ();
	}

	Destruct ();
}

void AThrowableBase::Multicast_ExplodeEffects_Implementation ()
{

}

void AThrowableBase::Destruct_Implementation ()
{
	if (HasAuthority ())
	{
		Destroy ();
	}
}
