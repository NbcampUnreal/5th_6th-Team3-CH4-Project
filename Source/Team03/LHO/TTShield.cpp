// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TTShield.h"
#include "LHO/TTPickupComponent.h"
#include "Character/TTPlayerCharacter.h"

ATTShield::ATTShield ()
{
	bReplicates = true;
	SetReplicateMovement ( true );

	PrimaryActorTick.bCanEverTick = false;

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );
	SetRootComponent ( PickupComponent );

	PickupComponent->SetIsReplicated ( true );
}

void ATTShield::BeginPlay ()
{
	Super::BeginPlay ();

	PickupComponent->OnPickUp.AddDynamic ( this , &ThisClass::HandleOnPickUp );
}

void ATTShield::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
{
	if (!IsValid ( InPickUpCharacter ))
	{
		return;
	}

	if (PickupComponent)
	{
		PickupComponent->SetSimulatePhysics ( false );

		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	}

	FAttachmentTransformRules AttachmentRules ( EAttachmentRule::SnapToTarget , true );

	bool bResult = AttachToComponent ( InPickUpCharacter->GetMesh () , AttachmentRules , FName ( TEXT ( "hand_lSocket" ) ) );
}
