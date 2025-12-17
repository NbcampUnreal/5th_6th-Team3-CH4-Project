// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TTShield.h"
#include "LHO/TTPickupComponent.h"
#include "Character/TTPlayerCharacter.h"

ATTShield::ATTShield ()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );
	SetRootComponent ( PickupComponent );
}

void ATTShield::BeginPlay ()
{
	Super::BeginPlay ();

	PickupComponent->OnPickUp.AddDynamic ( this , &ThisClass::HandleOnPickUp );
}

void ATTShield::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
{
	if (IsValid ( InPickUpCharacter ) == false)
	{
		return;
	}

	FAttachmentTransformRules AttachmentRules ( EAttachmentRule::SnapToTarget , true );
	AttachToComponent ( InPickUpCharacter->GetMesh () , AttachmentRules , FName ( TEXT ( "hand_lSocket" ) ) );
	SetActorEnableCollision ( false );
	PickupComponent->SetSimulatePhysics ( false );
}
