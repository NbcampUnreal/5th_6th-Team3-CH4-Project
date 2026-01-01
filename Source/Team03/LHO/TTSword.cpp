// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TTSword.h"
#include "LHO/TTPickupComponent.h"
#include "Character/TTPlayerCharacter.h"

ATTSword::ATTSword ()
{
	bReplicates = true;
	SetReplicateMovement ( true );
	PrimaryActorTick.bCanEverTick = false;

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );
	SetRootComponent ( PickupComponent );
	
	PickupComponent->SetIsReplicated ( true );

	WeaponRowName = "Sword";
}

void ATTSword::HandleOnThrowAway ()
{
	if (HasAuthority ())
	{
		MulticastThrowAway ();
	}
}

void ATTSword::MulticastThrowAway_Implementation ()
{
	DetachFromActor ( FDetachmentTransformRules::KeepWorldTransform );

	if (PickupComponent)
	{
		PickupComponent->SetSimulatePhysics ( true );
		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );

		PickupComponent->SetCollisionResponseToChannel ( ECC_Pawn , ECR_Ignore );

		FVector ThrowDir = GetActorForwardVector () + FVector ( 0 , 0 , 0.5f );
		PickupComponent->AddImpulse ( ThrowDir * 300.0f , NAME_None , true );

		GetWorld ()->GetTimerManager ().SetTimer
		(
			CollisionRecoveryTimerHandle ,
			this ,
			&ATTSword::EnablePickupCollision ,
			1.0f ,
			false
		);
	}
}

void ATTSword::BeginPlay ()
{
	Super::BeginPlay ();

	PickupComponent->OnPickUp.AddDynamic ( this , &ThisClass::HandleOnPickUp );
}

void ATTSword::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
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

	bool bResult = AttachToComponent ( InPickUpCharacter->GetMesh () , AttachmentRules , FName ( TEXT ( "hand_rSocket" ) ) );
}

void ATTSword::EnablePickupCollision ()
{
	if (PickupComponent)
	{
		PickupComponent->SetCollisionResponseToChannel ( ECC_Pawn , ECR_Overlap );
	}
}
