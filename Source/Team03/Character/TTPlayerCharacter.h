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
class UTTPickupComponent;
class ATTSword;
class ATTShield;
class ATTSword02;
class ATTShield02;
class ATTAxe;
class ATTHammer;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnPlayerDiedDelegate );

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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerDiedDelegate OnPlayerDied;

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
	TObjectPtr<UInputAction> InputDance1;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputDance2;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputDance3;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputDance4;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputAction> InputDance5;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputMappingContext> IMC_Character;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Animation" )
	TArray<TObjectPtr<UAnimMontage>> DanceMontages;

	UPROPERTY ( EditAnywhere , Category = "Sound" )
	TArray<TObjectPtr<USoundBase>> DanceSounds;

	void StopDanceAndMusic ();
	UPROPERTY ()
	TObjectPtr<UAudioComponent> CurrentDanceAudio;
	UFUNCTION ( Server , Reliable )
	void ServerPlayDance ( int32 Index );

	UFUNCTION ( NetMulticast , Reliable )
	void MulticastPlayDance ( int32 Index );
public:
	virtual void SetupPlayerInputComponent ( class UInputComponent* PlayerInputComponent ) override;
	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	UTTPickupComponent* GetOverlappingPickupComponent ()
	{
		return OverlappingPickupComponent;
	}

	void SetOverlappingPickupComponent ( UTTPickupComponent* InComp )
	{
		OverlappingPickupComponent = InComp;
	}

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
	void PickUp(const FInputActionValue& Value);
	void ThrowAway ( const FInputActionValue& Value );
	void Dance1 ( const FInputActionValue& Value );
	void Dance2 ( const FInputActionValue& Value );
	void Dance3 ( const FInputActionValue& Value );
	void Dance4 ( const FInputActionValue& Value );
	void Dance5 ( const FInputActionValue& Value );

	UFUNCTION(Server, Reliable)
	void ServerSetBlocking ( bool bNewBlocking );

	UFUNCTION(Server, Reliable)
	void ServerPickUp ();

	UFUNCTION(Server, Reliable)
	void ServerThrowAway ();

	void OnAnimation();
	void EndAnimation();

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

	UPROPERTY ( VisibleInstanceOnly , Category = "Interaction" )
	UTTPickupComponent* OverlappingPickupComponent;

	UPROPERTY ( VisibleInstanceOnly , Replicated , Category = "Interaction" )
	TObjectPtr<ATTSword> CurrentSword;

	UPROPERTY ( VisibleInstanceOnly , Replicated , Category = "Interaction" )
	TObjectPtr<ATTShield> CurrentShield;

	UPROPERTY ( VisibleInstanceOnly , Replicated , Category = "Interaction" )
	TObjectPtr<ATTSword02> CurrentSword02;

	UPROPERTY ( VisibleInstanceOnly , Replicated , Category = "Interaction" )
	TObjectPtr<ATTAxe> CurrentAxe;

	UPROPERTY ( VisibleInstanceOnly , Replicated , Category = "Interaction" )
	TObjectPtr<ATTHammer> CurrentHammer;

	UPROPERTY ( VisibleInstanceOnly , Replicated , Category = "Interaction" )
	TObjectPtr<ATTShield02> CurrentShield02;



	bool IsHoldingAnything () const;
	bool IsHoldingWeapon () const;
	bool IsHoldingShield () const;
	bool HasAnyWeapon () const;

	UPROPERTY ( VisibleAnywhere , Replicated , Category = "State" )
	bool bIsBlocking;
	
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

	UPROPERTY(VisibleInstanceOnly, Replicated, Category ="State")
	bool bIsInvincibility;


public:

	virtual void BeginPlay () override;
	virtual void Tick ( float DeltaTime ) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TSubclassOf<ACharacter> GhostClass;

	FTimerHandle DeathTimerHandle;

	void StartGhost ();

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float SpectateDelayTime = 3.0f;



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
	void SetInvincibility (bool bNewState);
	UFUNCTION(Server, Reliable)
	void ServerSetInvincibility (bool bNewState);

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
	UPROPERTY ( Replicated )
	int32 CurrentComboCount = 0;

	bool bIsNowAttacking = false;
	UPROPERTY ( Replicated )
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

	UFUNCTION ( Server , Reliable )
	void ServerStartAttack ();

	UFUNCTION ( NetMulticast , Reliable )
	void MulticastPlayAttackMontage ( int32 ComboIndex );

	UFUNCTION ( Server , Reliable )
	void ServerRequestNextCombo ();

	UPROPERTY ()
	bool bHasHitThisCombo = false;
#pragma endregion

#pragma region HP

public:
	bool IsDead () const { return bIsDead; }
protected:
	UPROPERTY ( VisibleAnywhere , ReplicatedUsing = OnRep_IsDead , BlueprintReadOnly )
	uint8 bIsDead : 1;

#pragma endregion
#pragma region Throw_Glass
public:

	void ApplySlow ( float Amount , float Duration );
	void ApplyStun ( float Amount );
	void AddThrowable ( AThrowableBase* Throwable );

	UFUNCTION()
	void OnRep_IsDead ();
	UFUNCTION(Server, Reliable)
	void ServerDeath ();

protected:

	float BaseWalkSpeed;

	FTimerHandle SlowTimerHandle;
	FTimerHandle StunTimerHandle;

	void ClearSlow ();
	void EndStun ();

	UPROPERTY ()
	AThrowableBase* CurrentThrowable = nullptr;

#pragma endregion

#pragma region Throw_BOMBs
public:



protected:


#pragma endregion

};
