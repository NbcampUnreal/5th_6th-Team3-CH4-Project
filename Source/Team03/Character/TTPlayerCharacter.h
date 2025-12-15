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
	void Attack( const FInputActionValue& Value );
	void InChat ();
	void ESCMenu();
	void TempKey ();
	void SprintStart ();
	void SprintEnd ();
	void PlayerBlocking ( const FInputActionValue& Value );

	UPROPERTY(Replicated)
	FRotator TargetRotation;

	UFUNCTION(Server, Reliable)
	void ServerSprintStart ();
	UFUNCTION(Server, REliable)
	void ServerSprintEnd ();
	void SetSprintSpeed ( bool bIsSprinting );

	UFUNCTION(Server, Unreliable)
	void ServerSetRotation ( FRotator NewRotation );

	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	TObjectPtr<UAnimMontage> AttackMeleeMontage;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	TObjectPtr<UAnimMontage> BlockingMontage;
	
#pragma endregion

private:
	UPROPERTY ( EditAnywhere )
	float WalkSpeed;
	UPROPERTY ( EditAnywhere )
	float SprintSpeed;

	UPROPERTY(EditAnywhere, Replicated)
	float MaxHP;
	UPROPERTY ( EditAnywhere , Replicated )
	float CurrentHP;
	UPROPERTY ( EditAnywhere )
	float MaxSturn;
	UPROPERTY ( EditAnywhere )
	float CurrentSturn;

public:

	virtual void BeginPlay () override;
	virtual void Tick ( float DeltaTime ) override;

#pragma region GetSet
public:
	void SetMaxHP (float amount);
	float GetMaxHP ();
	void SetCurrentHP ( float amount );
	float GetCurrentHP ();
	void SetMaxSturn ( float amount );
	float GetMaxSturn ();
	void SetCurrentSturn (  float amount );
	float GetCurrentSturn ();

#pragma endregion

#pragma region MeshChange
public:
	void InitializeMesh ( class ATTPlayerState* TTPS );

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

#pragma endregion

#pragma region Attack

public:
	UFUNCTION ()
	void HandleOnCheckHit ();
	UFUNCTION ()
	void HandleOnCheckInputAttack ();

	virtual void BeginAttack ();

	UFUNCTION ()
	virtual void EndAttack ( UAnimMontage* InMontage , bool bInterruped );
protected:
	FString AttackAnimMontageSectionPrefix = FString ( TEXT ( "Attack" ) );

	int32 MaxComboCount = 3;

	int32 CurrentComboCount = 0;

	bool bIsNowAttacking = false;

	bool bIsAttackKeyPressed = false;

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;
#pragma endregion
};
