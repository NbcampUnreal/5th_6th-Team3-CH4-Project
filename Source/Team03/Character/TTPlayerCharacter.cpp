//TTPlyaerCharacter.cpp

#include "TTPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ATTPlayerCharacter::ATTPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent> ( TEXT ( "SpringArm" ) );
	SpringArm->SetupAttachment ( GetRootComponent () );
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;	
	
	Camera = CreateDefaultSubobject<UCameraComponent> ( TEXT ( "Camera" ) );
	Camera->SetupAttachment ( SpringArm );

	bUseControllerRotationYaw = false;

	GetCharacterMovement ()->bOrientRotationToMovement = true;
	GetCharacterMovement ()->RotationRate=FRotator( 0.0f , 500.0f , 0.0f );
}

void ATTPlayerCharacter::SetupPlayerInputComponent ( UInputComponent* PlayerInputComponent )
{
}

void ATTPlayerCharacter::BeginPlay ()
{
}

void ATTPlayerCharacter::Move ( const FInputActionValue& Value )
{
}

void ATTPlayerCharacter::Look ( const FInputActionValue& Value )
{
}

void ATTPlayerCharacter::Attack ()
{
}

