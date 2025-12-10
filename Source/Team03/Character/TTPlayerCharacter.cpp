//TTPlyaerCharacter.cpp

#include "TTPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Controller/TTPlayerController.h"
#include "Net/UnrealNetwork.h"


ATTPlayerCharacter::ATTPlayerCharacter()
{
	WalkSpeed = 600.f;
	SprintSpeed = 1000.f;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Head = CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Head" ) );
	Head->SetupAttachment ( GetRootComponent () );

	SpringArm = CreateDefaultSubobject<USpringArmComponent> ( TEXT ( "SpringArm" ) );
	SpringArm->SetupAttachment ( GetRootComponent () );
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;	
	
	Camera = CreateDefaultSubobject<UCameraComponent> ( TEXT ( "Camera" ) );
	Camera->SetupAttachment ( SpringArm );

	bUseControllerRotationYaw = false;

	GetCharacterMovement ()->bOrientRotationToMovement = false;
	GetCharacterMovement ()->RotationRate=FRotator( 0.0f , 1080.0f , 0.0f );

	TargetRotation = FRotator::ZeroRotator;

	HeadMeshToReplicate = nullptr;
	BodyMeshToReplicate = nullptr;
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
		EnhancedInputComponent->BindAction ( InputBlocking , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::PlayerBlocking );
		EnhancedInputComponent->BindAction ( InputSprint , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::SprintStart );
		EnhancedInputComponent->BindAction ( InputSprint , ETriggerEvent::Completed , this , &ATTPlayerCharacter::SprintEnd );

		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Jump );
		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Completed , this , &ATTPlayerCharacter::StopJumping );
		
		EnhancedInputComponent->BindAction ( InputEnter , ETriggerEvent::Started , this , &ATTPlayerCharacter::InChat );
		EnhancedInputComponent->BindAction ( InputESC, ETriggerEvent::Started , this , &ATTPlayerCharacter::ESCMenu);
		EnhancedInputComponent->BindAction ( InputTempKey , ETriggerEvent::Started , this , &ATTPlayerCharacter::TempKey);

	}
}

void ATTPlayerCharacter::BeginPlay ()
{
	Super::BeginPlay ();
	UE_LOG ( LogTemp , Warning , TEXT ( "BeginPlay" ) );

	APlayerController* PlayerController = Cast<APlayerController> ( GetController () );

	if (IsValid ( PlayerController ) == true)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> ( PlayerController->GetLocalPlayer() );

		if (IsValid ( Subsystem ) == true)
		{
			Subsystem->AddMappingContext ( IMC_Character , 0 );
		}
	}
}

void ATTPlayerCharacter::Move ( const FInputActionValue& Value )
{
	FVector2D MovementVector = Value.Get<FVector2D> ();
	if (IsValid ( Controller ) == true)
	{
		const FRotator Rotation = Controller->GetControlRotation ();
		const FRotator RotationYaw ( 0 , Rotation.Yaw , 0 );
		const FVector Forward = FRotationMatrix ( RotationYaw ).GetUnitAxis ( EAxis::X );
		const FVector Right = FRotationMatrix ( RotationYaw ).GetUnitAxis ( EAxis::Y );

		AddMovementInput ( Forward , MovementVector.X );
		AddMovementInput ( Right , MovementVector.Y );


		FVector DesiredDirection = (Forward * MovementVector.X) + (Right * MovementVector.Y);

		if (!DesiredDirection.IsNearlyZero ())
		{
			TargetRotation = DesiredDirection.Rotation ();
		}

	}
}

void ATTPlayerCharacter::Tick ( float DeltaTime )
{
	Super::Tick ( DeltaTime );

	float TurnSpeed = GetCharacterMovement ()->RotationRate.Yaw;
	FRotator NewRotation = FMath::RInterpConstantTo ( GetActorRotation () , TargetRotation , DeltaTime , TurnSpeed );

	SetActorRotation ( NewRotation );
}

void ATTPlayerCharacter::Look ( const FInputActionValue& Value )
{
	FVector2D MouseVector = Value.Get<FVector2D> ();

	if (IsValid ( Controller ) == true)
	{
		AddControllerYawInput ( MouseVector.X );
		AddControllerPitchInput ( MouseVector.Y );
	}
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

void ATTPlayerCharacter::TempKey ()
{
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetController () ))
	{
		PC->ActivateSelectedSkeletalMesh ();
	}
}

void ATTPlayerCharacter::SprintStart ()
{
	GetCharacterMovement ()->MaxWalkSpeed = SprintSpeed;
}
void ATTPlayerCharacter::SprintEnd ()
{
	GetCharacterMovement ()->MaxWalkSpeed = WalkSpeed;
}

void ATTPlayerCharacter::PlayerBlocking ()
{
}

#pragma region MeshChange
void ATTPlayerCharacter::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( ATTPlayerCharacter , HeadMeshToReplicate );
	DOREPLIFETIME ( ATTPlayerCharacter , BodyMeshToReplicate );
}

bool ATTPlayerCharacter::ServerChangeHeadMesh_Validate ( USkeletalMesh* NewMesh )
{
	return true;
}

void ATTPlayerCharacter::ServerChangeHeadMesh_Implementation ( USkeletalMesh* NewMesh )
{

	HeadMeshToReplicate = NewMesh;
	OnRep_HeadMesh ();

}

bool ATTPlayerCharacter::ServerChangeBodyMesh_Validate ( USkeletalMesh* NewMesh )
{
	return true;
}

void ATTPlayerCharacter::ServerChangeBodyMesh_Implementation ( USkeletalMesh* NewMesh )
{
	BodyMeshToReplicate = NewMesh;

	OnRep_BodyMesh ();
}


void ATTPlayerCharacter::OnRep_HeadMesh ()
{
	if (HeadMeshToReplicate)
	{
		ChangeHead ( HeadMeshToReplicate );
	}
}

void ATTPlayerCharacter::OnRep_BodyMesh ()
{
	if (BodyMeshToReplicate)
	{
		ChangeBody ( BodyMeshToReplicate );
	}
}

void ATTPlayerCharacter::ChangeHead ( USkeletalMesh* NewMesh )
{
	if (IsValid ( Head ) && IsValid ( NewMesh ))
	{
		Head->SetSkeletalMesh ( NewMesh );
	}
}

void ATTPlayerCharacter::ChangeBody ( USkeletalMesh* NewMesh )
{
	if (IsValid ( GetMesh () ) && IsValid ( NewMesh ))
	{
		GetMesh ()->SetSkeletalMesh ( NewMesh );
	}
}

#pragma endregion
