//MapsGimmick.cpp

#include "MapsGimmick.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

AMapsGimmick::AMapsGimmick()
{
	PrimaryActorTick.bCanEverTick = true;

	GasDetectionVolume = CreateDefaultSubobject<USphereComponent> ( TEXT ( "GasDetectionVolume" ) );
	RootComponent = GasDetectionVolume;

	GasDetectionVolume->OnComponentBeginOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapBegin );
	GasDetectionVolume->OnComponentEndOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapEnd );


}

void AMapsGimmick::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMapsGimmick::OnOverlapBegin ( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{

}

void AMapsGimmick::OnOverlapEnd ( UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex )
{

}

void AMapsGimmick::GasDamage ()
{

}


