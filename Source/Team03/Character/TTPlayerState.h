// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTPlayerState.generated.h"

/**
 * 
 */
UENUM ( BlueprintType )
enum class Teams : uint8
{
	None UMETA ( DisplayName = "None" ) ,
	Red UMETA ( DisplayName = "Red" ) ,
	Blue UMETA ( DisplayName = "Blue" )
};

UCLASS ()
class TEAM03_API ATTPlayerState : public APlayerState
{
	GENERATED_BODY ()
public:
	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	void SetTeam ( Teams NewTeam );
	Teams GetTeam () const { return Team; };
public:
	UPROPERTY ( Replicated )
	USkeletalMesh* PersistedHeadMesh;
	UPROPERTY ( Replicated )
	USkeletalMesh* PersistedBodyMesh;

	UPROPERTY(ReplicatedUsing = OnRep_UserNickname)
	FString UserNickname;

	UPROPERTY(Replicated)
	FName SelectedCharacterRowName;

	UPROPERTY ( ReplicatedUsing = OnRep_Team )
	Teams Team = Teams::Blue;

	UFUNCTION()
	void OnRep_UserNickname();
	UFUNCTION()
	void OnRep_Team ();
};