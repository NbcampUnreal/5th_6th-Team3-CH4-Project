// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TTPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UFloatingPawnMovement;
//class USpringArmComponent;
//class UCameraComponent;
struct FInputActionValue;

UCLASS()
class TEAM03_API ATTPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATTPawn();

protected:
	UPROPERTY ( EditDefaultsOnly , Category = "ASXPlayerPawn" )
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY ( EditDefaultsOnly , Category = "ASXPlayerPawn" )
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY ( EditDefaultsOnly , Category = "ASXPlayerPawn" )
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovementComponent;

	//UPROPERTY ( EditDefaultsOnly , Category = "ASXPlayerPawn" )
	//TObjectPtr<USpringArmComponent> SpringArmComponent;

	//UPROPERTY ( EditDefaultsOnly , Category = "ASXPlayerPawn" )
	//TObjectPtr<UCameraComponent> CameraComponent;

	//virtual void SetupPlayerInputComponent ( class UInputComponent* PlayerInputComponent ) override;

	//UFUNCTION ()
	//void Move ( const FInputActionValue& value );
	//UFUNCTION ()
	//void StartJump ( const FInputActionValue& value );
	//UFUNCTION ()
	//void StopJump ( const FInputActionValue& value );
	//UFUNCTION ()
	//void StartRun ( const FInputActionValue& value );
	//UFUNCTION ()
	//void StopRun ( const FInputActionValue& value );


};
