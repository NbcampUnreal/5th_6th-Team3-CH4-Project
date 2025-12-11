//MapsGimmick.cpp

#include "MapsGimmick.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Gas_Damage.h"

AMapsGimmick::AMapsGimmick ()
{
	PrimaryActorTick.bCanEverTick = true;

	GasDetectionVolume = CreateDefaultSubobject<USphereComponent> ( TEXT ( "GasDetectionVolume" ) );
	RootComponent = GasDetectionVolume;

	GasDetectionVolume->SetCollisionProfileName ( TEXT ( "Trigger" ) );
	GasDetectionVolume->OnComponentBeginOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapBegin );
	GasDetectionVolume->OnComponentEndOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapEnd );

}

void AMapsGimmick::BeginPlay ()
{
	Super::BeginPlay ();

	UE_LOG ( LogTemp , Warning , TEXT ( "MapsGimmick BeginPlay" ) );
	
	if (HasAuthority ())
	{
		float StartDelay = 10.0f;
		GetWorldTimerManager ().SetTimer (
			GasStartTimer ,
			this ,
			&AMapsGimmick::StartGasDamage ,
			StartDelay ,
			false
		);
	}
}

void AMapsGimmick::StartGasDamage ()
{
	if (!HasAuthority ()) return;

	bGasActive = true;

	UE_LOG ( LogTemp , Warning , TEXT ( "Gas damage started" ) );
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
	if (!bGasActive) return;

	ACharacter* Char = Cast<ACharacter> ( OtherActor );
	if (Char && !ActorsInGas.Contains ( Char ))
	{
		ActorsInGas.Add ( Char );

		UE_LOG ( LogTemp , Warning , TEXT ( "Character entered gas area" ) );

		if (ActorsInGas.Num () == 1)
		{
			GetWorldTimerManager ().SetTimer (
				GasDamageTimerHandle ,
				this ,
				&AMapsGimmick::GasDamage ,
				DamageInterval ,
				true ,
				0.0f
			);
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
	if (!bGasActive) return;

	ACharacter* Char = Cast<ACharacter> ( OtherActor );
	if (Char && ActorsInGas.Contains ( Char ))
	{
		ActorsInGas.Remove ( Char );

		UE_LOG ( LogTemp , Warning , TEXT ( "Character exited gas area" ) );

		if (ActorsInGas.Num () == 0)
		{
			GetWorldTimerManager ().ClearTimer ( GasDamageTimerHandle );
		}
	}
}

void AMapsGimmick::GasDamage ()
{
	if (!HasAuthority ()) return;
	if (!bGasActive) return;

	for (ACharacter* Char : ActorsInGas)
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "Applying gas damage to character" ) );

		UGameplayStatics::ApplyDamage (
			Char ,
			DamagePerTick ,
			nullptr ,
			this ,
			UGas_Damage::StaticClass ()
		);
	}
}

void AMapsGimmick::Tick ( float DeltaTime )
{
	Super::Tick ( DeltaTime );
}

void AMapsGimmick::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( AMapsGimmick , bGasActive );
}