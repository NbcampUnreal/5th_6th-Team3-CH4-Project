#include "LHO/SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent> ( TEXT ( "Scene" ) );
	SetRootComponent ( Scene );

	SpawningBox = CreateDefaultSubobject<UBoxComponent> ( TEXT ( "SpawningBox" ) );
	SpawningBox->SetupAttachment ( Scene );

	ItemDataTable = nullptr;
}

void ASpawnVolume::SpawnRandomItem ()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem ())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get ())
		{
			SpawnItem ( ActualClass );
		}
	}
}

FVector ASpawnVolume::GetRandomPointInVolume () const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent ();
	FVector BoxOrigin = SpawningBox->GetComponentLocation ();

	return BoxOrigin + FVector 
	(
		FMath::FRandRange ( -BoxExtent.X , BoxExtent.X ) ,
		FMath::FRandRange ( -BoxExtent.Y , BoxExtent.Y ) ,
		FMath::FRandRange ( -BoxExtent.Z , BoxExtent.Z )
	);
}

FItemSpawnRow* ASpawnVolume::GetRandomItem () const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString ( TEXT ( "ItemSpawnContext" ) );
	ItemDataTable->GetAllRows ( ContextString , AllRows );

	if (AllRows.IsEmpty ()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row) // Row가 유효한지 확인
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange ( 0.0f , TotalChance );
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;
}

void ASpawnVolume::SpawnItem ( TSubclassOf<AActor> ItemClass )
{
	if (!ItemClass) return;

	GetWorld ()->SpawnActor<AActor> 
		(
		ItemClass ,
		GetRandomPointInVolume () ,
		FRotator::ZeroRotator
	);
}

