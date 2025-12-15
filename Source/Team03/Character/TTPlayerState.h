// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTPlayerState.generated.h"

/**
 * 
 */
USTRUCT ()
struct FCharacterMeshData
{
	GENERATED_BODY ()
public:
	UPROPERTY ()
	TSoftObjectPtr<USkeletalMesh> HeadMeshID;

	UPROPERTY ()
	TSoftObjectPtr<USkeletalMesh> BodyMeshID;
};

UCLASS ()
class TEAM03_API ATTPlayerState : public APlayerState
{
	GENERATED_BODY ()
public:
	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	void SetMeshData ( FCharacterMeshData InData );

	UFUNCTION ()
	void OnRep_MeshData ();
public:
	UPROPERTY ( ReplicatedUsing = OnRep_MeshData )
	FCharacterMeshData MeshData;

	UPROPERTY(ReplicatedUsing = OnRep_UserNickname)
	FString UserNickname;

	UPROPERTY(Replicated)
	FName SelectedCharacterRowName;

	UFUNCTION()
	void OnRep_UserNickname();
};