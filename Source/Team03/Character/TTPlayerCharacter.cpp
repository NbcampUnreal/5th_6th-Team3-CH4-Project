//TTPlyaerCharacter.cpp

#include "TTPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Controller/TTPlayerController.h"
#include "TTPlayerState.h"
#include "Save/TTSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "LHO/TTAnimInstance.h"
#include "Camera/PlayerCameraManager.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"

ATTPlayerCharacter::ATTPlayerCharacter()
{
	WalkSpeed = 400.f;
	SprintSpeed = 600.f;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Head = CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Head" ) );
	Head->SetupAttachment ( GetRootComponent () );

	SpringArm = CreateDefaultSubobject<USpringArmComponent> ( TEXT ( "SpringArm" ) );
	SpringArm->SetupAttachment ( GetRootComponent () );
	SpringArm->TargetArmLength = 700.f;
	SpringArm->bUsePawnControlRotation = true;	
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	SpringArm->CameraLagMaxDistance = 100.0f;
	
	Camera = CreateDefaultSubobject<UCameraComponent> ( TEXT ( "Camera" ) );
	Camera->SetupAttachment ( SpringArm );

	bUseControllerRotationYaw = false;

	GetCharacterMovement ()->bOrientRotationToMovement = false;
	GetCharacterMovement ()->RotationRate = FRotator ( 0.0f , 1440.0f , 0.0f );
	TargetRotation = FRotator::ZeroRotator;

}

void ATTPlayerCharacter::BeginPlay ()
{
	Super::BeginPlay ();
	UE_LOG ( LogTemp , Warning , TEXT ( "BeginPlay" ) );

	//UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	//if (IsValid ( AnimInstance ) == true)
	//{
	//	AnimInstance->OnCheckHit.AddDynamic ( this , &ThisClass::HandleOnCheckHit );
	//}

	ATTPlayerController* PlayerController = Cast<ATTPlayerController> ( GetController () );

	if (IsValid ( PlayerController ) == true)
	{
		PlayerController->SetControlRotation (FRotator(0.f , -70.f , 0.f ));
		PlayerController->PlayerCameraManager->ViewPitchMin = -80.f ;
		PlayerController->PlayerCameraManager->ViewPitchMax = -30.f ;

		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> ( PlayerController->GetLocalPlayer() );

		if (IsValid ( Subsystem ) == true)
		{
			Subsystem->AddMappingContext ( IMC_Character , 0 );
		}
	}
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
	{
		const UTTCharactorSkeletalMeshSelect* CDOBody = GetDefault<UTTCharactorSkeletalMeshSelect> ();
		const UTTCharactorHeadSkeletalSelect* CDOHead = GetDefault<UTTCharactorHeadSkeletalSelect> ();
		if (0 < CDOBody->PlayerCharacterSkeletalPaths.Num () && 0 < CDOHead->PlayerCharacterHeadSkeletalPaths.Num ())
		{
			FSoftObjectPath CurrentSkeletalPath = CDOBody->PlayerCharacterSkeletalPaths[0];
			TSoftObjectPtr<USkeletalMesh> BodyInstance ( CurrentSkeletalPath );
			FSoftObjectPath CurrentSkeletalHeadPath = CDOHead->PlayerCharacterHeadSkeletalPaths[0];
			TSoftObjectPtr<USkeletalMesh> HeadInstance ( CurrentSkeletalHeadPath );


			PS->SetHeadMeshData ( HeadInstance );
			PS->SetBodyMeshData ( BodyInstance );
		}
	}
}

void ATTPlayerCharacter::Tick ( float DeltaTime )
{
	Super::Tick ( DeltaTime );

	FRotator CurrentRotation = GetActorRotation ();
	FRotator NewRotation;

	if (IsLocallyControlled ())
	{
		float TurnSpeed = GetCharacterMovement ()->RotationRate.Yaw;
		NewRotation = FMath::RInterpConstantTo ( CurrentRotation , TargetRotation , DeltaTime , TurnSpeed );
	}
	else
	{
		float TurnSpeed = GetCharacterMovement ()->RotationRate.Yaw;
		NewRotation = FMath::RInterpConstantTo ( CurrentRotation , TargetRotation , DeltaTime , TurnSpeed );
	}
	SetActorRotation ( NewRotation );
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
	{
		GetMesh ();
		USkeletalMesh* SCHead = Cast<USkeletalMesh> ( PS->HeadMeshID.Get () );
		USkeletalMesh* SCBody = Cast<USkeletalMesh> ( PS->BodyMeshID.Get () );
		if (IsValid ( SCHead ) && IsValid ( SCBody ))
		{
			Head->SetSkeletalMesh ( SCHead );
			GetMesh ()->SetSkeletalMesh ( SCBody );
		}
		if (ATTPlayerController* TTPC = Cast<ATTPlayerController> ( GetController () ))
		{
			TTPC->ServerRequestChangeHeadMesh ( SCHead );
			TTPC->ServerRequestChangeBodyMesh ( SCBody );
		}
	}
}


void ATTPlayerCharacter::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME_CONDITION ( ATTPlayerCharacter , TargetRotation, COND_SkipOwner );
}

#pragma region Input
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
		EnhancedInputComponent->BindAction ( InputESC , ETriggerEvent::Started , this , &ATTPlayerCharacter::ESCMenu );
		EnhancedInputComponent->BindAction ( InputTempKey , ETriggerEvent::Started , this , &ATTPlayerCharacter::TempKey );

	}
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
			if (!TargetRotation.Equals(DesiredDirection.Rotation(), 0.1f ))
			{
				TargetRotation = DesiredDirection.Rotation ();

				ServerSetRotation ( TargetRotation );
			}
		}

	}
}

void ATTPlayerCharacter::Attack ( const FInputActionValue& Value )
{
	if (GetCharacterMovement ()->IsFalling () == true)
	{
		return;
	}

	//UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	//if (IsValid ( AnimInstance ) == true && IsValid ( AttackMeleeMontage ) == true && AnimInstance->Montage_IsPlaying ( AttackMeleeMontage ) == false)
	//{
	//	AnimInstance->Montage_Play ( AttackMeleeMontage );
	//}
	if (0 == CurrentComboCount)
	{
		BeginAttack ();
	}
	else
	{
		ensure ( FMath::IsWithinInclusive<int32> ( CurrentComboCount , 1 , MaxComboCount ) );
		bIsAttackKeyPressed = true;
	}
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
	SetSprintSpeed ( true );
	ServerSprintStart ();
}
void ATTPlayerCharacter::SprintEnd ()
{
	SetSprintSpeed ( false );
	ServerSprintEnd ();
}

void ATTPlayerCharacter::PlayerBlocking ( const FInputActionValue& Value )
{
	if (GetCharacterMovement ()->IsFalling () == true)
	{
		return;
	}

	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	if (IsValid ( AnimInstance ) == true && IsValid ( BlockingMontage ) == true && AnimInstance->Montage_IsPlaying ( BlockingMontage ) == false)
	{
		AnimInstance->Montage_Play ( BlockingMontage );
	}
}

void ATTPlayerCharacter::SetSprintSpeed ( bool bIsSprinting )
{
	if (bIsSprinting)
	{
		GetCharacterMovement ()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement ()->MaxWalkSpeed = WalkSpeed;
	}
}

void ATTPlayerCharacter::ServerSetRotation_Implementation ( FRotator NewRotation )
{
	TargetRotation = NewRotation;
}


void ATTPlayerCharacter::ServerSprintStart_Implementation ()
{
	SetSprintSpeed ( true );
}

void ATTPlayerCharacter::ServerSprintEnd_Implementation ()
{
	SetSprintSpeed (false);
}

#pragma endregion

#pragma region MeshChange
void ATTPlayerCharacter::ApplyHeadMeshData ( TSoftObjectPtr<USkeletalMesh> InData )
{
	if (Head && InData)
		Head->SetSkeletalMesh ( InData.Get() );
}

void ATTPlayerCharacter::ApplyBodyMeshData ( TSoftObjectPtr<USkeletalMesh> InData )
{
	if (GetMesh () && InData)
		GetMesh ()->SetSkeletalMesh ( InData.Get () );
}

#pragma endregion


#pragma region Attack

void ATTPlayerCharacter::HandleOnCheckHit ()
{
	UKismetSystemLibrary::PrintString ( this , TEXT ( "HandleOnCheckHit()" ) );
}
void ATTPlayerCharacter::HandleOnCheckInputAttack ()
{
	UKismetSystemLibrary::PrintString ( this , TEXT ( "HandleOnCheckInputAttack()" ) );
	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	checkf ( IsValid ( AnimInstance ) == true , TEXT ( "Invalid AnimInstance" ) );

	if (bIsAttackKeyPressed == true)
	{
		CurrentComboCount = FMath::Clamp ( CurrentComboCount + 1 , 1 , MaxComboCount );

		FName NextSectionName = *FString::Printf ( TEXT ( "%s%02d" ) , *AttackAnimMontageSectionPrefix , CurrentComboCount );
		AnimInstance->Montage_JumpToSection ( NextSectionName , AttackMeleeMontage );
		bIsAttackKeyPressed = false;
	}
}
void ATTPlayerCharacter::BeginAttack ()
{
	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	checkf ( IsValid ( AnimInstance ) == true , TEXT ( "Invalid AnimInstance" ) );

	/*GetCharacterMovement ()->SetMovementMode ( EMovementMode::MOVE_None );*/
	bIsNowAttacking = true;
	if (IsValid ( AnimInstance ) == true && IsValid ( AttackMeleeMontage ) == true && AnimInstance->Montage_IsPlaying ( AttackMeleeMontage ) == false)
	{
		AnimInstance->Montage_Play ( AttackMeleeMontage );
	}

	CurrentComboCount = 1;

	if (OnMeleeAttackMontageEndedDelegate.IsBound () == false)
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject ( this , &ThisClass::EndAttack );
		AnimInstance->Montage_SetEndDelegate ( OnMeleeAttackMontageEndedDelegate , AttackMeleeMontage );
	}
}

void ATTPlayerCharacter::EndAttack ( UAnimMontage* InMontage , bool bInterruped )
{
	ensureMsgf ( CurrentComboCount != 0 , TEXT ( "CurrentComboCount == 0" ) );

	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;
	//GetCharacterMovement ()->SetMovementMode ( EMovementMode::MOVE_Walking );

	if (OnMeleeAttackMontageEndedDelegate.IsBound () == true)
	{
		OnMeleeAttackMontageEndedDelegate.Unbind ();
	}
}
#pragma endregion