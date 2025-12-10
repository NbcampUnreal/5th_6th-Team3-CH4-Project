//MapsGimmick.cpp

#include "MapsGimmick.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

AMapsGimmick::AMapsGimmick ()
{
	PrimaryActorTick.bCanEverTick = true;

	GasDetectionVolume = CreateDefaultSubobject<USphereComponent> ( TEXT ( "GasDetectionVolume" ) );
	RootComponent = GasDetectionVolume;

	GasDetectionVolume->SetCollisionProfileName ( TEXT ( "Trigger" ) );
	GasDetectionVolume->OnComponentBeginOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapBegin );
	GasDetectionVolume->OnComponentEndOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapEnd );

}

void AMapsGimmick::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMapsGimmick::OnOverlapBegin ( 
	UPrimitiveComponent* OverlappedComp , 
	AActor* OtherActor , 
	UPrimitiveComponent* OtherComp , 
	int32 OtherBodyIndex , 
	bool bFromSweep , 
	const FHitResult& SweepResult )
{
	if (!HasAuthority ()) return;

	ACharacter* Char = Cast<ACharacter> ( OtherActor );
	if (Char && !ActorsInGas.Contains ( Char ) )
	{
		ActorsInGas.Add ( Char );
		if ( ActorsInGas.Num () == 1 )
		{
			GetWorldTimerManager ().SetTimer ( 
				GasDamageTimerHandle , 
				this , 
				&AMapsGimmick::GasDamage , 
				DamageInterval , 
				true , 
				0.0f );
		}
	}
}

void AMapsGimmick::OnOverlapEnd ( 
	UPrimitiveComponent* OverlappedComp ,
	AActor* OtherActor ,
	UPrimitiveComponent* OtherComp ,
	int32 OtherBodyIndex )
{
	if (!HasAuthority ()) return;

	ACharacter* Char = Cast<ACharacter> ( OtherActor );
	if (Char && !ActorsInGas.Contains ( Char ))
	{
		ActorsInGas.Remove ( Char );
		if ( ActorsInGas.Num () == 0 )
		{
			GetWorldTimerManager ().ClearTimer ( GasDamageTimerHandle );
		}
	}
}

void AMapsGimmick::GasDamage ()
{
	if (!HasAuthority ()) return;

	for ( ACharacter* Char : ActorsInGas )
	{
		UGameplayStatics::ApplyDamage ( 
			Char , 
			DamagePerTick , 
			nullptr , 
			this , 
			nullptr );
	}
}

void AMapsGimmick::Tick ( float DeltaTime )
{

}

