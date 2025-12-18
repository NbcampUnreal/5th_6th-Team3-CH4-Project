// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TTSword.h"
#include "LHO/TTPickupComponent.h"
#include "Character/TTPlayerCharacter.h"

ATTSword::ATTSword ()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );
	SetRootComponent ( PickupComponent );
}

void ATTSword::BeginPlay ()
{
	Super::BeginPlay ();

	PickupComponent->OnPickUp.AddDynamic ( this , &ThisClass::HandleOnPickUp );
}

void ATTSword::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
{
	if (IsValid ( InPickUpCharacter ) == false)
	{
		return;
	}

	FAttachmentTransformRules AttachmentRules ( EAttachmentRule::SnapToTarget , true );
	AttachToComponent ( InPickUpCharacter->GetMesh () , AttachmentRules , FName ( TEXT ( "hand_rSocket" ) ) );
	SetActorEnableCollision ( false );
	PickupComponent->SetSimulatePhysics ( false );

	//InPickUpCharacter->CurrentWeapon = this;
}

