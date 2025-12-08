#include "TTPawn.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
//#include "GameFramework/SpringArmComponent.h"
//#include "Camera/CameraComponent.h"


ATTPawn::ATTPawn ()
{
	PrimaryActorTick.bCanEverTick = false;

	float PawnHalfHeight = 90.f;
	float PawnRadius = 40.f;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent> ( TEXT ( "CapsuleComponent" ) );
	SetRootComponent ( CapsuleComponent );
	CapsuleComponent->SetCapsuleHalfHeight ( PawnHalfHeight );
	CapsuleComponent->SetCapsuleRadius ( PawnRadius );

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "SkeletalMeshComponent" ) );
	SkeletalMeshComponent->SetupAttachment ( RootComponent );
	FVector PivotPosition ( 0.f , 0.f , -PawnHalfHeight );
	FRotator PivotRotation ( 0.f , -90.f , 0.f );
	SkeletalMeshComponent->SetRelativeLocationAndRotation ( PivotPosition , PivotRotation );

	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement> ( TEXT ( "FloatingPawnMovementComponent" ) );

	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent> ( TEXT ( "SpringArmComponent" ) );
	//SpringArmComponent->SetupAttachment ( RootComponent );
	//SpringArmComponent->TargetArmLength = 800.f;
	//SpringArmComponent->SetRelativeRotation ( FRotator ( -60.f , 0.f , 0.f ) );

	//CameraComponent = CreateDefaultSubobject<UCameraComponent> ( TEXT ( "CameraComponent" ) );
	//CameraComponent->SetupAttachment ( SpringArmComponent );

}

//void ATTPawn::SetupPlayerInputComponent ( UInputComponent* PlayerInputComponent )
//{
//	Super::SetupPlayerInputComponent ( PlayerInputComponent );
//
//	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent> ( PlayerInputComponent ))
//	{
//		if (ATTPlayerController* PlayerController = Cast<ATTPlayerController> ( GetController () ))
//		{
//			if (PlayerController->MoveAction)
//			{
//				EnhancedInput->BindAction (
//					PlayerController->MoveAction ,
//					ETriggerEvent::Triggered ,
//					this ,
//					&ATTPawn::Move
//				);
//			}
//
//			if (PlayerController->JumpAction)
//			{
//				EnhancedInput->BindAction (
//					PlayerController->JumpAction ,
//					ETriggerEvent::Triggered ,
//					this ,
//					&ATTPawn::StartJump
//				);
//
//				EnhancedInput->BindAction (
//					PlayerController->JumpAction ,
//					ETriggerEvent::Completed ,
//					this ,
//					&ATTPawn::StopJump
//				);
//			}
//
//			if (PlayerController->SprintAction)
//			{
//				EnhancedInput->BindAction (
//					PlayerController->SprintAction ,
//					ETriggerEvent::Triggered ,
//					this ,
//					&ATTPawn::StartRun
//				);
//				EnhancedInput->BindAction (
//					PlayerController->SprintAction ,
//					ETriggerEvent::Completed ,
//					this ,
//					&ATTPawn::StopRun
//				);
//			}
//		}
//	}
//}
//
//void ATTPawn::Move ( const FInputActionValue& value )
//{
//}
//
//void ATTPawn::StartJump ( const FInputActionValue& value )
//{
//}
//
//void ATTPawn::StopJump ( const FInputActionValue& value )
//{
//}
//
//void ATTPawn::StartRun ( const FInputActionValue& value )
//{
//}
//
//void ATTPawn::StopRun ( const FInputActionValue& value )
//{
//}