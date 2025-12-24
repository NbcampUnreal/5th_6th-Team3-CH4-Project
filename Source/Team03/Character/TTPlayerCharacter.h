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
class AThrowableBase;
class USceneCaptureComponent2D;

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

	UPROPERTY( EditDefaultsOnly , BlueprintReadOnly, Category = "SceneCapture")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY ( EditDefaultsOnly , Category = "UI Capture" )
	UTextureRenderTarget2D* CaptureRT;
#pragma region Input
public:
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
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputPlayerKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input")
	TObjectPtr<UInputAction> InputSprint;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputBlocking;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InputThrowAway;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputPickUp;
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
	void JumpStart ();
	void JumpEnd ();
	void PickUpStart ();
	void PickUpEnd ();
	//인게임 UI추가
	void OnAnimation ();
	void EndAnimation ();

	UPROPERTY(Replicated)
	FRotator TargetRotation;

	UFUNCTION(Server, Reliable)
	void ServerSprintStart ();
	UFUNCTION(Server, Reliable)
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
	float MaxStun;
	UPROPERTY ( EditAnywhere )
	float CurrentStun;


public:

	virtual void BeginPlay () override;
	virtual void Tick ( float DeltaTime ) override;

#pragma region GetSet
public:
	void SetMaxHP (float amount);
	float GetMaxHP ();
	void SetCurrentHP (float amount );
	float GetCurrentHP ();
	void SetMaxStun ( float amount );
	float GetMaxStun ();
	void SetCurrentStun (  float amount );
	float GetCurrentStun ();

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
	UFUNCTION(Server, Reliable)
	void ServerHandleOnCheckHit ();
	UFUNCTION ()
	void HandleOnCheckInputAttack ();

	virtual void BeginAttack ();

	UFUNCTION ()
	virtual void EndAttack ( UAnimMontage* InMontage , bool bInterruped );

	virtual float TakeDamage ( float DamageAmount , struct FDamageEvent const& DamageEvent , class AController* EventInstigator , AActor* DamageCauser ) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponData ( FName NewWeaponName );

	void KnockOut ();

	//static int32 ShowAttackMeleeDebug;
protected:
	FString AttackAnimMontageSectionPrefix = FString ( TEXT ( "Attack" ) );

	int32 MaxComboCount = 5;

	int32 CurrentComboCount = 0;

	bool bIsNowAttacking = false;

	bool bIsAttackKeyPressed = false;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , ReplicatedUsing = OnRep_IsStunned )
	bool bIsStunned;

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	float AttackMeleeRange = 50.f;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	float AttackMeleeRadius = 20.f;


	float StunDuration = 10.0f;

	UPROPERTY ( EditAnywhere , Category = "Weapon" )
	TObjectPtr<UDataTable> WeaponData;

	FName WeaponName;

	UPROPERTY(ReplicatedUsing = OnRep_ServerRagdollLocation)
	FVector ServerRagdollLocation;

	UPROPERTY(Replicated)
	FVector ServerRagdollVelocity;

	UPROPERTY(Replicated)
	FRotator ServerRagdollRotation;

	UPROPERTY(Replicated)
	FVector ServerRagdollAngularVelocity;

	UFUNCTION ()
	void OnRep_IsStunned();

	void WakeUp ();

	UFUNCTION()
	void OnRep_ServerRagdollLocation ();

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Animation" )
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Sound" )
	TObjectPtr<USoundBase> HitSound;

	UFUNCTION ( NetMulticast , Unreliable )
	void MulticastPlayHitMontage ();

#pragma endregion

#pragma region HP

public:
	bool IsDead () const { return bIsDead; }
protected:
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly )
	uint8 bIsDead : 1;

#pragma endregion Throw
public:

	void ApplySlow ( float Amount , float Duration );
	void ApplyStun ( float Amount );
	void AddThrowable ( AThrowableBase* Throwable );

protected:

	float BaseWalkSpeed;

	FTimerHandle SlowTimerHandle;
	FTimerHandle StunTimerHandle;

	void ClearSlow ();
	void EndStun ();

	UPROPERTY ()
	AThrowableBase* CurrentThrowable = nullptr;
};
