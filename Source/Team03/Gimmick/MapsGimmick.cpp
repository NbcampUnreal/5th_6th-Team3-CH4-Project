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
	bReplicates = true;

	GasDetectionVolume = CreateDefaultSubobject<USphereComponent> ( TEXT ( "GasDetectionVolume" ) );
	RootComponent = GasDetectionVolume;

	GasDetectionVolume->InitSphereRadius ( 400.f );

	GasDetectionVolume->SetCollisionEnabled ( ECollisionEnabled::QueryOnly );
	GasDetectionVolume->SetCollisionResponseToAllChannels ( ECR_Ignore );
	GasDetectionVolume->SetCollisionResponseToChannel ( ECC_Pawn , ECR_Overlap );

	//GasDetectionVolume->SetCollisionProfileName ( TEXT ( "Trigger" ) );
	GasDetectionVolume->OnComponentBeginOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapBegin );
	GasDetectionVolume->OnComponentEndOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapEnd );

}

void AMapsGimmick::BeginPlay ()
{
	Super::BeginPlay ();

	UE_LOG ( LogTemp , Warning , TEXT ( "MapsGimmick BeginPlay" ) );
	UE_LOG ( LogTemp , Warning , TEXT ( "StartGasDamage called | Authority: %d" ) , HasAuthority () );

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

	TArray<AActor*> OverlappingActors;
	GasDetectionVolume->GetOverlappingActors (
		OverlappingActors ,
		ACharacter::StaticClass () 
	);

	for (AActor* Actor : OverlappingActors)
	{
		if (ACharacter* Char = Cast<ACharacter> ( Actor ))
		{
			ActorsInGas.AddUnique ( Char );
		}
	}

	if (ActorsInGas.Num () > 0)
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

void AMapsGimmick::OnOverlapBegin (
	UPrimitiveComponent* OverlappedComp ,
	AActor* OtherActor ,
	UPrimitiveComponent* OtherComp ,
	int32 OtherBodyIndex ,
	bool bFromSweep ,
	const FHitResult& SweepResult )
{
	UE_LOG (LogTemp ,Warning ,	TEXT ( "Overlap Begin | Other: %s | Authority: %d" ) ,*OtherActor->GetName () ,HasAuthority ());

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
	UE_LOG (LogTemp ,Warning ,TEXT ( "GasDamage Tick | ActorsInGas: %d | bGasActive: %d | Authority: %d" ) ,ActorsInGas.Num () ,bGasActive ,HasAuthority ());

	if (!HasAuthority ()) return;
	if (!bGasActive) return;

	for (int32 i = ActorsInGas.Num () - 1; i >= 0; --i)
	{
		ACharacter* Char = ActorsInGas[i];

		UE_LOG (LogTemp ,Warning ,TEXT ( "Applying gas damage to %s" ) ,*Char->GetName () );

		if (!IsValid ( Char ))
		{
			ActorsInGas.RemoveAt ( i );
			continue;
		}

		UGameplayStatics::ApplyDamage (
			Char ,
			DamagePerTick ,
			nullptr ,
			this ,
			UGas_Damage::StaticClass ()
		);
	}

	if (ActorsInGas.Num () == 0)
	{
		GetWorldTimerManager ().ClearTimer ( GasDamageTimerHandle );
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