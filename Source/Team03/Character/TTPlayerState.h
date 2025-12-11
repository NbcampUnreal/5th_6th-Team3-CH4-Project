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
public:
	UPROPERTY ( Replicated )
	USkeletalMesh* PersistedHeadMesh;
	UPROPERTY ( Replicated )
	USkeletalMesh* PersistedBodyMesh;

<<<<<<< HEAD
	UPROPERTY(ReplicatedUsing = OnRep_UserNickname)
	FString UserNickname;

	UPROPERTY(Replicated)
	FName SelectedCharacterRowName;

	UFUNCTION()
	void OnRep_UserNickname();
=======
>>>>>>> InGameUI_PCY
};