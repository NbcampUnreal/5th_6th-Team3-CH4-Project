// TTPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TTPlayerCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UAnimMontage;

UCLASS()
class TEAM03_API ATTPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATTPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArm")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Camera" )
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head")
	TObjectPtr<USkeletalMeshComponent> Head;

#pragma region Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InputMove;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputLook;
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InputJump;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputAttack;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputEnter;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputESC;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputTempKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputAction> InputSprint;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputBlocking;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputMappingContext> IMC_Character;

public:
	virtual void SetupPlayerInputComponent ( class UInputComponent* PlayerInputComponent ) override;
	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

protected:
	void Move ( const FInputActionValue& Value );
	void Look ( const FInputActionValue& Value );
	void Attack( const FInputActionValue& InValue );
	void InChat ();
	void ESCMenu();
	void TempKey ();
	void SprintStart ();
	void SprintEnd ();
<<<<<<< HEAD
	void PlayerBlocking ();

	UPROPERTY(Replicated)
	FRotator TargetRotation;

	UFUNCTION(Server, Reliable)
	void ServerSprintStart ();
	UFUNCTION(Server, REliable)
	void ServerSprintEnd ();
	void SetSprintSpeed ( bool bIsSprinting );

	UFUNCTION(Server, Unreliable)
	void ServerSetRotation ( FRotator NewRotation );
	
=======
	void PlayerBlocking ( const FInputActionValue& InValue );
	FRotator TargetRotation;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	TObjectPtr<UAnimMontage> PlayerBlockingMontage;
>>>>>>> Character-LHO
#pragma endregion

private:
	UPROPERTY ( EditAnywhere )
	float WalkSpeed;
	UPROPERTY ( EditAnywhere )
	float SprintSpeed;

public:

	virtual void BeginPlay () override;
	virtual void Tick ( float DeltaTime ) override;

#pragma region MeshChange
public:


	UFUNCTION ( Server , Reliable , WithValidation )
	void ServerChangeHeadMesh ( USkeletalMesh* NewMesh );
	UFUNCTION ( Server , Reliable , WithValidation )
	void ServerChangeBodyMesh ( USkeletalMesh* NewMesh );

	void ChangeHead ( USkeletalMesh* NewMesh );
	void ChangeBody ( USkeletalMesh* NewMesh );

	UFUNCTION ()
	void OnRep_HeadMesh ();

	UFUNCTION ()
	void OnRep_BodyMesh ();

private:
	UPROPERTY ( ReplicatedUsing = OnRep_HeadMesh )
	USkeletalMesh* HeadMeshToReplicate;

	UPROPERTY ( ReplicatedUsing = OnRep_BodyMesh )
	USkeletalMesh* BodyMeshToReplicate;

<<<<<<< HEAD
#pragma endregion

#pragma region SaveData
	public:
	void SavePlayerSaveData ( const FString& SlotName , int32 UserIndex );
	void LoadPlayerSaveData ( const FString& SlotName , int32 UserIndex );
=======
	UPROPERTY(EditAnywhere)
	float WalkSpeed;
	UPROPERTY ( EditAnywhere)
	float SprintSpeed;
>>>>>>> Character-LHO
#pragma endregion
};
