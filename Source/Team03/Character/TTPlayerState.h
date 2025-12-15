// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTPlayerState.generated.h"

/**
 * 
 */
UCLASS ()
class TEAM03_API ATTPlayerState : public APlayerState
{
	GENERATED_BODY ()
public:
	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	void SetHeadMeshData ( TSoftObjectPtr<USkeletalMesh> InData);
	void SetBodyMeshData ( TSoftObjectPtr<USkeletalMesh> InData );

	UFUNCTION ()
	void OnRep_HeadMeshData ();

	UFUNCTION ()
	void OnRep_BodyMeshData ();
public:
	UPROPERTY ( ReplicatedUsing = OnRep_HeadMeshData )
	TSoftObjectPtr<USkeletalMesh> HeadMeshID;

	UPROPERTY ( ReplicatedUsing = OnRep_BodyMeshData )
	TSoftObjectPtr<USkeletalMesh> BodyMeshID;

	UPROPERTY(ReplicatedUsing = OnRep_UserNickname)
	FString UserNickname;

	UPROPERTY(Replicated)
	FName SelectedCharacterRowName;

	UFUNCTION()
	void OnRep_UserNickname();
};