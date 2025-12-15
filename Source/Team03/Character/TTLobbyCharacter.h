// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TTLobbyCharacter.generated.h"

UCLASS()
class TEAM03_API ATTLobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATTLobbyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMeshComponent> Head;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMeshComponent> Body; // Usually the main Mesh is body, but if separated:

	// Replication - Mesh Assets
	UPROPERTY(ReplicatedUsing = OnRep_HeadMesh)
	USkeletalMesh* HeadMesh;

	UPROPERTY(ReplicatedUsing = OnRep_BodyMesh)
	USkeletalMesh* BodyMesh;

	// Replication - Indices
	UPROPERTY(Replicated)
	int32 CurrentHeadIndex;

	UPROPERTY(Replicated)
	int32 CurrentBodyIndex;

	UFUNCTION()
	void OnRep_HeadMesh();

	UFUNCTION()
	void OnRep_BodyMesh();

	// Server RPCs - Index-Based
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeHeadMeshByIndex(int32 MeshIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeBodyMeshByIndex(int32 MeshIndex);

	// Helpers
	void ChangeHead(int32 Index);
	void ChangeBody(int32 Index);

	// Getters
	int32 GetCurrentHeadIndex() const { return CurrentHeadIndex; }
	int32 GetCurrentBodyIndex() const { return CurrentBodyIndex; }

};
