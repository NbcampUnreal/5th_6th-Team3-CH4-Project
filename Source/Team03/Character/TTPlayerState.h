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
	Teams GetTeam () const { return Team; }
	FString GetUserNickname () const { return UserNickname; }
public:
	UPROPERTY ( Replicated )
	USkeletalMesh* PersistedHeadMesh;
	UPROPERTY ( Replicated )
	USkeletalMesh* PersistedBodyMesh;

	UPROPERTY ( Replicated )
	UTexture2D* PortraitTexture;

	UPROPERTY(ReplicatedUsing = OnRep_UserNickname)
	FString UserNickname;

	UPROPERTY(Replicated)
	FName SelectedCharacterRowName;

	UPROPERTY ( ReplicatedUsing = OnRep_Team )
	Teams Team = Teams::None;

	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool bIsReady = false;

    UPROPERTY(Replicated)
    bool bIsHost = false;

	UFUNCTION()
	void OnRep_UserNickname();
	UFUNCTION()
	void OnRep_Team ();
	UFUNCTION()
	void OnRep_IsReady();
};