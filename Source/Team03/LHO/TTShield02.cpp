// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TTShield02.h"
#include "LHO/TTPickupComponent.h"
#include "Character/TTPlayerCharacter.h"

ATTShield02::ATTShield02 ()
{
	bReplicates = true;
	SetReplicateMovement ( true );

	PrimaryActorTick.bCanEverTick = false;

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );
	SetRootComponent ( PickupComponent );

	PickupComponent->SetIsReplicated ( true );

	WeaponRowName = "Shield";
}

void ATTShield02::BeginPlay ()
{
	Super::BeginPlay ();

	PickupComponent->OnPickUp.AddDynamic ( this , &ThisClass::HandleOnPickUp );
}

void ATTShield02::HandleOnThrowAway ()
{
	DetachFromActor ( FDetachmentTransformRules::KeepWorldTransform );

	if (PickupComponent)
	{
		PickupComponent->SetSimulatePhysics ( true );
		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );

		FVector ThrowDir = GetActorForwardVector () + FVector ( 0 , 0 , 0.5f );
		PickupComponent->AddImpulse ( ThrowDir * 300.0f , NAME_None , true );
	}
}

void ATTShield02::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
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
