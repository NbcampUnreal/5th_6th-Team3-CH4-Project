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
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Input" )
	TObjectPtr<UInputMappingContext> IMC_Character;

public:
	virtual void SetupPlayerInputComponent ( class UInputComponent* PlayerInputComponent ) override;


protected:
	void Move ( const FInputActionValue& Value );
	void Look ( const FInputActionValue& Value );
	void Attack();
	void InChat ();
	void ESCMenu();
	void TempKey ();
	FRotator TargetRotation;

	
#pragma endregion

public:

	virtual void BeginPlay () override;

#pragma region MeshChange
public:
	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

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
};
