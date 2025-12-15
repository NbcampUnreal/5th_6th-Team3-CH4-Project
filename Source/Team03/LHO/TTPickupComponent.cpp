// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TTPickupComponent.h"
#include "Character/TTPlayerCharacter.h"

UTTPickupComponent::UTTPickupComponent ()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTTPickupComponent::BeginPlay ()
{
	Super::BeginPlay ();

	OnComponentBeginOverlap.AddDynamic ( this , &ThisClass::HandleOnComponentBeginOverlap );
}

void UTTPickupComponent::HandleOnComponentBeginOverlap ( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	ATTPlayerCharacter* OverlappedCharacter = Cast<ATTPlayerCharacter> ( OtherActor );
	if (IsValid ( OverlappedCharacter ) == true)
	{
		OnPickUp.Broadcast ( OverlappedCharacter );

		OnComponentBeginOverlap.RemoveAll ( this );
	}
}