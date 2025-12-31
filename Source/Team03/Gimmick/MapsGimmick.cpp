//MapsGimmick.cpp

#include "MapsGimmick.h"
#include "Components/BoxComponent.h"
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

	GasDetectionVolume = CreateDefaultSubobject<UBoxComponent> ( TEXT ( "GasDetectionVolume" ) );
	RootComponent = GasDetectionVolume;

	GasDetectionVolume->SetBoxExtent ( GasBoxExtent );

	GasDetectionVolume->SetCollisionEnabled ( ECollisionEnabled::QueryOnly );
	GasDetectionVolume->SetCollisionResponseToAllChannels ( ECR_Ignore );
	GasDetectionVolume->SetCollisionResponseToChannel ( ECC_Pawn , ECR_Overlap );

	GasDetectionVolume->OnComponentBeginOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapBegin );
	GasDetectionVolume->OnComponentEndOverlap.AddDynamic ( this , &AMapsGimmick::OnOverlapEnd );

	GasMesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "GasMesh" ) );
	GasMesh->SetupAttachment ( RootComponent );

	GasMesh->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	GasMesh->SetHiddenInGame ( true );
}

void AMapsGimmick::BeginPlay ()
{
	Super::BeginPlay ();

	if (HasAuthority ())
	{
		GetWorldTimerManager ().SetTimer (
			GasStartTimer ,
			this ,
			&AMapsGimmick::StartGasDamage ,
			GasStartDelay ,
			false
		);
	}
}

void AMapsGimmick::StartGasDamage ()
{
	UE_LOG ( LogTemp , Warning , TEXT ( "Gas damage started" ) );

	if (!HasAuthority ()) return;

	bGasActive = true;
	OnRep_GasActive ();

	TArray<AActor*> OverlappingActors;
	GasDetectionVolume->GetOverlappingActors (OverlappingActors);

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

void AMapsGimmick::OnRep_GasActive ()
{
	if (GasMesh)
	{
		GasMesh->SetHiddenInGame ( !bGasActive );
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

	if (!HasAuthority ()) return;
	if (!bGasActive) return;

	ACharacter* Char = Cast<ACharacter> ( OtherActor );
	if (Char && !ActorsInGas.Contains ( Char ))
	{
		ActorsInGas.Add ( Char );

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

	for (int32 i = ActorsInGas.Num () - 1; i >= 0; --i)
	{
		ACharacter* Char = ActorsInGas[i];

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