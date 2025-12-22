// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TTLobbyCharacter.generated.h"

class USkeletalMeshComponent;
class UWidgetComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class USkeletalMesh;

UCLASS()
class TEAM03_API ATTLobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATTLobbyCharacter();

protected:
#pragma region Life Cycle
	virtual void BeginPlay() override;
#pragma endregion

public:	
#pragma region Life Cycle
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region Components
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMeshComponent> Head;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMeshComponent> Body;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<class UWidgetComponent> NicknameWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    TObjectPtr<UStaticMeshComponent> TeamIndicatorMesh;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> IndicatorMID;
#pragma endregion

#pragma region Replication - Mesh Assets
	// 리플리케이션 - 메쉬 에셋
	UPROPERTY(ReplicatedUsing = OnRep_HeadMesh)
	USkeletalMesh* HeadMesh;

	UPROPERTY(ReplicatedUsing = OnRep_BodyMesh)
	USkeletalMesh* BodyMesh;
#pragma endregion

#pragma region Replication - Indices
	// 리플리케이션 - 인덱스
	UPROPERTY(Replicated)
	int32 CurrentHeadIndex;

	UPROPERTY(Replicated)
	int32 CurrentBodyIndex;
#pragma endregion

#pragma region Replication Callbacks
	UFUNCTION()
	void OnRep_HeadMesh();

	UFUNCTION()
	void OnRep_BodyMesh();
#pragma endregion

#pragma region Server RPCs
	// 서버 RPC - 인덱스 기반
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeHeadMeshByIndex(int32 MeshIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeBodyMeshByIndex(int32 MeshIndex);
#pragma endregion

#pragma region Helpers
	// 헬퍼
	void ChangeHead(int32 Index);
	void ChangeBody(int32 Index);

	// Getters
	int32 GetCurrentHeadIndex() const { return CurrentHeadIndex; }
	int32 GetCurrentBodyIndex() const { return CurrentBodyIndex; }
#pragma endregion
};
