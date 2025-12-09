//TTPlyaerCharacter.cpp

#include "TTPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Controller/TTPlayerController.h"



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
	Super::SetupPlayerInputComponent ( PlayerInputComponent );

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent> ( PlayerInputComponent );
	if (IsValid ( EnhancedInputComponent ) == true)
	{
		EnhancedInputComponent->BindAction ( InputMove , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Move );
		EnhancedInputComponent->BindAction ( InputLook , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Look );
		EnhancedInputComponent->BindAction ( InputAttack , ETriggerEvent::Started , this , &ATTPlayerCharacter::Attack );

		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Jump );
		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Completed , this , &ATTPlayerCharacter::StopJumping );
		
		EnhancedInputComponent->BindAction ( InputEnter , ETriggerEvent::Started , this , &ATTPlayerCharacter::InChat );
		EnhancedInputComponent->BindAction (InputESC, ETriggerEvent::Started , this , &ATTPlayerCharacter::ESCMenu);

	}
}

void ATTPlayerCharacter::BeginPlay ()
{
}

void ATTPlayerCharacter::Move ( const FInputActionValue& Value )
{
	FVector2D MovementVector = Value.Get<FVector2D> ();
	if (IsValid ( Controller ) == true)
	{

	}
}

void ATTPlayerCharacter::Look ( const FInputActionValue& Value )
{
}

void ATTPlayerCharacter::Attack ()
{
}

void ATTPlayerCharacter::InChat ()
{
	if(ATTPlayerController* PC = Cast<ATTPlayerController>( GetController () ) )
	{
		PC->ActivateChatBox ();
	}
}

void ATTPlayerCharacter::ESCMenu()
{
	if (ATTPlayerController* PC = Cast<ATTPlayerController>(GetController()))
	{
		PC->ActivateESCMenu();
	}
}

