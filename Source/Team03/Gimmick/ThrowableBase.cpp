// ThrowableBase.cpp

#include "ThrowableBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Character/TTPlayerCharacter.h"
#include "LHO/TTPickupComponent.h"

AThrowableBase::AThrowableBase ()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "MeshComp" ) );
	RootComponent = MeshComp;

	MeshComp->SetSimulatePhysics ( true );
	MeshComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
	MeshComp->SetCollisionObjectType ( ECC_PhysicsBody );
	MeshComp->SetNotifyRigidBodyCollision ( true );
	MeshComp->SetGenerateOverlapEvents ( false );

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );

	bReplicates = true;
	SetReplicateMovement ( true );

	bDestroyed = false;
	bAllowPickUp = false;
}

void AThrowableBase::BeginPlay ()
{
	Super::BeginPlay ();

	if (MeshComp)
	{
		MeshComp->OnComponentHit.AddDynamic (
			this ,
			&AThrowableBase::OnHit
		);
	}

	if (PickupComponent)
	{
		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	}
}

void AThrowableBase::GetLifetimeReplicatedProps (TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( AThrowableBase , bDestroyed );

	DOREPLIFETIME ( AThrowableBase , bAllowPickUp );

}

void AThrowableBase::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
{
	if (!InPickUpCharacter) return;

	if (!HasAuthority ()) return;

	if (!bAllowPickUp) return;
	
	if (OwnerCharacter) return;

	OwnerCharacter = InPickUpCharacter;
	SetOwner ( InPickUpCharacter );

	SetActorHiddenInGame ( true );
	SetActorEnableCollision ( false );

	InPickUpCharacter->AddThrowable ( this );
}

void AThrowableBase::OnHit (UPrimitiveComponent* HitComp ,AActor* OtherActor ,UPrimitiveComponent* OtherComp ,FVector NormalImpulse ,const FHitResult& Hit)
{

}

void AThrowableBase::ServerThrow_Implementation (const FVector& Direction ,float Power)
{
	Throw ( Direction , Power );
}

void AThrowableBase::Throw (const FVector& Direction ,float Power)
{
	if (!MeshComp) return;

	bAllowPickUp = true;

	if (PickupComponent)
	{
		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::QueryOnly );
	}

	SetActorHiddenInGame ( false );
	SetActorEnableCollision ( true );

	MeshComp->AddImpulse ( Direction * Power , NAME_None , true );
}

void AThrowableBase::Explode_Implementation ()
{

}

void AThrowableBase::Destruct_Implementation ()
{
	if (bDestroyed) return;

	bDestroyed = true;

	if (HasAuthority ())
	{
		Destroy ();
	}
}
