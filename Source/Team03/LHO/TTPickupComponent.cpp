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
	OnComponentEndOverlap.AddDynamic ( this , &ThisClass::HandleOnComponentEndOverlap );
}

void UTTPickupComponent::HandleOnComponentBeginOverlap ( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	ATTPlayerCharacter* OverlappedCharacter = Cast<ATTPlayerCharacter> ( OtherActor );
	if (IsValid ( OverlappedCharacter ) == true)
	{
		OverlappedCharacter->SetOverlappingPickupComponent ( this );
	}
}

void UTTPickupComponent::HandleOnComponentEndOverlap ( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex )
{
	ATTPlayerCharacter* OverlappedCharacter = Cast<ATTPlayerCharacter> ( OtherActor );
	if (IsValid ( OverlappedCharacter ))
	{

		if (OverlappedCharacter->GetOverlappingPickupComponent () == this)
		{
			OverlappedCharacter->SetOverlappingPickupComponent ( nullptr );
		}
	}
}

void UTTPickupComponent::ForcePickUp ( ATTPlayerCharacter* InCharacter )
{
	OnPickUp.Broadcast ( InCharacter );
}