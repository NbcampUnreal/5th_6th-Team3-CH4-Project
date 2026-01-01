#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;
class UDataTable;

UCLASS ()
class TEAM03_API ASpawnVolume : public AActor
{
	GENERATED_BODY ()

public:
	ASpawnVolume ();

	UFUNCTION ( BlueprintCallable , Category = "Spawning" )
	void SpawnRandomItem ();

	UFUNCTION ( BlueprintCallable , Category = "Spawning" )
	void StartSpawnTimer ();

	UFUNCTION ( BlueprintCallable , Category = "Spawning" )
	void StopSpawnTimer ();

protected:

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Spawning" )
	USceneComponent* Scene;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Spawning" )
	UBoxComponent* SpawningBox;


	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Spawning" )
	UDataTable* ItemDataTable;


	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Spawning|Time" )
	float SpawnInterval = 5.f;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Spawning|Time" )
	float InitialSpawnDelay = 2.f;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Spawning|Time" )
	float SpawnDuration = 30.f;

	virtual void BeginPlay () override;

	FVector GetRandomPointInVolume () const;
	FItemSpawnRow* GetRandomItem () const;
	void SpawnItem ( TSubclassOf<AActor> ItemClass );

	FTimerHandle SpawnTimerHandle;
};
