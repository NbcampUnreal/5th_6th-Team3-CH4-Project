//TTPlyaerCharacter.cpp

#include "TTPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Controller/TTPlayerController.h"
#include "Character/TTPlayerState.h"
#include "Save/TTSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "LHO/TTAnimInstance.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Team03.h"
#include "TTWeaponData.h"
#include "Components/CapsuleComponent.h"
#include "LHO/TTPickupComponent.h"
#include "LHO/TTShield.h"
#include "LHO/TTSword.h"
#include "Gimmick/Gas_Damage.h"
#include "LHO/TTAxe.h"
#include "LHO/TTHammer.h"
#include "LHO/TTShield02.h"
#include "LHO/TTSword02.h"
#include "InGameMode/InGameModeBase.h"
#include "Components/AudioComponent.h"
//int32 ATTPlayerCharacter::ShowAttackMeleeDebug = 0;
//
//FAutoConsoleVariableRef CVarShowAttackMeleeDebug (
//	TEXT ( "TT.ShowAttackMeleeDebug" ) ,
//	ATTPlayerCharacter::ShowAttackMeleeDebug ,
//	TEXT ( "" ) ,
//	ECVF_Cheat
//);

ATTPlayerCharacter::ATTPlayerCharacter () :
	WalkSpeed ( 200.f ) ,
	SprintSpeed ( 400.f ) ,
	MaxHP ( 100.f ) ,
	CurrentHP ( MaxHP ) ,
	MaxStun ( 100.f ) ,
	CurrentStun ( 0.f ) ,
	bIsStunned(false),
	bIsInvincibility(false)
{
	WeaponName = "Hand";
	WeaponData = nullptr;

	GetCharacterMovement ()->MaxWalkSpeed = WalkSpeed;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Head = CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Head" ) );
	Head->SetupAttachment ( GetMesh () );

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

	HeadMeshToReplicate = nullptr;
	BodyMeshToReplicate = nullptr;

	bIsDead = false;

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
	BaseWalkSpeed = GetCharacterMovement ()->MaxWalkSpeed;

	ATTPlayerController* PlayerController = Cast<ATTPlayerController> ( GetController () );

	if (IsValid ( PlayerController ) == true)
	{
		PlayerController->SetControlRotation ( FRotator ( 0.f , -70.f , 0.f ) );
		PlayerController->PlayerCameraManager->ViewPitchMin = -80.f;
		PlayerController->PlayerCameraManager->ViewPitchMax = -30.f;


		// ----- Outgame 담당자가 수정함 -----
		/*
		 * LoadPlayerSaveData 호출 비활성화:
		 * - SaveGame 로드가 PlayerState 데이터를 덮어쓰는 문제 발생
		 * - Seamless Travel을 사용하므로 PlayerState가 자동으로 유지됨
		 * PlayerController->LoadPlayerSaveData ( TEXT ( "MySaveSlot_01" ) , 0 );
		 */
		 // ----------------------------------


		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> ( PlayerController->GetLocalPlayer () );

		if (IsValid ( Subsystem ) == true)
		{
			Subsystem->AddMappingContext ( IMC_Character , 0 );
		}
	}
	// ----- Outgame 담당자가 수정함 -----
	if (HasAuthority ())
	{
		if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
		{
			if (USkeletalMesh* HeadMesh = PS->PersistedHeadMesh)
			{
				HeadMeshToReplicate = HeadMesh;
				OnRep_HeadMesh ();
			}

			if (USkeletalMesh* BodyMesh = PS->PersistedBodyMesh)
			{
				BodyMeshToReplicate = BodyMesh;
				OnRep_BodyMesh ();
			}
		}
	}
	// ----------------------------------


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

	//ATTPlayerState* PS = Cast<ATTPlayerState> ( GetPlayerState () );
	//if (IsValid ( PS->PersistedBodyMesh ) && IsValid ( PS->PersistedHeadMesh ) && IsValid( PS ))
	//{
	//	if ((GetMesh () != Cast<USkeletalMeshComponent> ( PS->PersistedBodyMesh )) || (Head != Cast<USkeletalMeshComponent> ( PS->PersistedHeadMesh )))
	//	{
	//		InitializeMesh ( PS );
	//	}
	//}
	if ((bIsStunned || bIsDead) && HasAuthority ())
	{
		ServerRagdollLocation = GetMesh ()->GetSocketLocation ( TEXT ( "pelvis" ) );
		ServerRagdollRotation = GetMesh ()->GetSocketRotation ( TEXT ( "pelvis" ) );
		ServerRagdollVelocity = GetMesh ()->GetPhysicsLinearVelocity ( TEXT ( "pelvis" ) );
		ServerRagdollAngularVelocity = GetMesh ()->GetPhysicsAngularVelocityInDegrees ( TEXT ( "pelvis" ) );
	}

}

void ATTPlayerCharacter::StartGhost ()
{
	if (APlayerController * PC = Cast<APlayerController> ( GetController () ))
	{
		if (GetWorld () && GhostClass)
		{
			FVector SpawnLoc = GetActorLocation ();
			FRotator SpawnRot = GetControlRotation ();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ACharacter* GhostChar = GetWorld ()->SpawnActor<ACharacter> ( GhostClass , SpawnLoc , SpawnRot , SpawnParams );

			if (GhostChar)
			{
				PC->Possess ( GhostChar );
			}
		}
	}

}

#pragma region Get,Set
void ATTPlayerCharacter::SetMaxHP ( float amount )
{
	MaxHP = amount;
}

float ATTPlayerCharacter::GetMaxHP ()
{
	return MaxHP;
}

void ATTPlayerCharacter::SetCurrentHP ( float amount )
{
	CurrentHP = amount;
}

float ATTPlayerCharacter::GetCurrentHP ()
{
	return CurrentHP;
}

void ATTPlayerCharacter::SetMaxStun ( float amount )
{
	MaxStun = amount;
}

float ATTPlayerCharacter::GetMaxStun ()
{
	return MaxStun;
}

void ATTPlayerCharacter::SetCurrentStun ( float amount )
{
	CurrentStun = amount;
}

float ATTPlayerCharacter::GetCurrentStun ()
{
	return CurrentStun;
}

void ATTPlayerCharacter::SetInvincibility ( bool bNewState )
{
	if (!HasAuthority ())
	{
		ServerSetInvincibility ( bNewState );
	}
	else
	{
		ServerSetInvincibility_Implementation (bNewState);
	}
}
void ATTPlayerCharacter::ServerSetInvincibility_Implementation ( bool bNewState )
{
	bIsInvincibility = bNewState;
}
#pragma endregion

void ATTPlayerCharacter::InitializeMesh ( ATTPlayerState* TTPS )
{
	ServerChangeHeadMesh ( TTPS->PersistedHeadMesh );
	ServerChangeBodyMesh ( TTPS->PersistedBodyMesh );
}

void ATTPlayerCharacter::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( ATTPlayerCharacter , HeadMeshToReplicate );
	DOREPLIFETIME ( ATTPlayerCharacter , BodyMeshToReplicate );
	DOREPLIFETIME ( ATTPlayerCharacter , bIsStunned );

	DOREPLIFETIME ( ATTPlayerCharacter , ServerRagdollLocation );
	DOREPLIFETIME ( ATTPlayerCharacter , ServerRagdollVelocity );
	DOREPLIFETIME ( ATTPlayerCharacter , ServerRagdollRotation );
	DOREPLIFETIME ( ATTPlayerCharacter , ServerRagdollAngularVelocity );

	DOREPLIFETIME ( ATTPlayerCharacter , CurrentSword );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentShield );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentSword02 );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentAxe );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentHammer );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentShield02 );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentComboCount );
	DOREPLIFETIME ( ATTPlayerCharacter , bIsAttackKeyPressed );

	DOREPLIFETIME ( ATTPlayerCharacter , bIsBlocking );

	DOREPLIFETIME ( ATTPlayerCharacter , bIsInvincibility );

	DOREPLIFETIME ( ATTPlayerCharacter , bIsDead );

	DOREPLIFETIME_CONDITION ( ATTPlayerCharacter , TargetRotation, COND_SkipOwner );

	//HP 레플리케이션 추가
	DOREPLIFETIME ( ATTPlayerCharacter , MaxHP );
	DOREPLIFETIME ( ATTPlayerCharacter , CurrentHP );
	
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
		EnhancedInputComponent->BindAction ( InputBlocking , ETriggerEvent::Started , this , &ATTPlayerCharacter::PlayerBlocking );
		EnhancedInputComponent->BindAction ( InputBlocking , ETriggerEvent::Completed , this , &ATTPlayerCharacter::PlayerBlocking );
		EnhancedInputComponent->BindAction ( InputSprint , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::SprintStart );
		EnhancedInputComponent->BindAction ( InputSprint , ETriggerEvent::Completed , this , &ATTPlayerCharacter::SprintEnd );

		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::JumpStart );
		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Completed , this , &ATTPlayerCharacter::JumpEnd );

		EnhancedInputComponent->BindAction ( InputEnter , ETriggerEvent::Started , this , &ATTPlayerCharacter::InChat );
		EnhancedInputComponent->BindAction ( InputESC , ETriggerEvent::Started , this , &ATTPlayerCharacter::ESCMenu );
		EnhancedInputComponent->BindAction ( InputTempKey , ETriggerEvent::Started , this , &ATTPlayerCharacter::TempKey );

		EnhancedInputComponent->BindAction ( InputPickUp , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::PickUp );
		EnhancedInputComponent->BindAction ( InputThrowAway , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::ThrowAway );
		EnhancedInputComponent->BindAction ( InputDance1 , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Dance1 );
		EnhancedInputComponent->BindAction ( InputDance2 , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Dance2);
		EnhancedInputComponent->BindAction ( InputDance3 , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Dance3 );
		EnhancedInputComponent->BindAction ( InputDance4 , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Dance4);
		EnhancedInputComponent->BindAction ( InputDance5 , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::Dance5 );

		EnhancedInputComponent->BindAction ( InputPlayerKey , ETriggerEvent::Started , this , &ATTPlayerCharacter::OnAnimation );
		EnhancedInputComponent->BindAction ( InputPlayerKey , ETriggerEvent::Completed , this , &ATTPlayerCharacter::EndAnimation );
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
	if (bIsStunned) return;
	if (bIsDead) return;
	UAnimInstance* AnimInstance = GetMesh ()->GetAnimInstance ();

	FVector2D MovementVector = Value.Get<FVector2D> ();
	if (!MovementVector.IsNearlyZero ())
	{
		StopDanceAndMusic ();
	}
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
			if (!TargetRotation.Equals ( DesiredDirection.Rotation () , 0.1f ))
			{
				TargetRotation = DesiredDirection.Rotation ();

				ServerSetRotation ( TargetRotation );
			}
		}

	}
}


void ATTPlayerCharacter::InChat ()
{
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetController () ))
	{
		PC->ActivateChatBox ();
	}
}

void ATTPlayerCharacter::ESCMenu ()
{
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetController () ))
	{
		PC->ActivateESCMenu ();
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
	bool bInputState = Value.Get<bool> ();

	if (bIsStunned) return;
	if (GetCharacterMovement ()->IsFalling () == true)
	{
		return;
	}
	if (bInputState)
	{
		StopDanceAndMusic ();
	}
	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	if (IsValid ( AnimInstance ) == true && IsValid ( BlockingMontage ) == true && AnimInstance->Montage_IsPlaying ( BlockingMontage ) == false)
	{
		AnimInstance->Montage_Play ( BlockingMontage );
	}

	ServerSetBlocking ( bInputState );
}

void ATTPlayerCharacter::JumpStart ()
{
	if (bIsStunned) return;
	StopDanceAndMusic ();
	Super::Jump ();
}

void ATTPlayerCharacter::JumpEnd ()
{
	if (bIsStunned) return;

	Super::StopJumping ();
}
void ATTPlayerCharacter::Dance1 ( const FInputActionValue& Value )
{
	ServerPlayDance ( 0 );
}
void ATTPlayerCharacter::Dance2 ( const FInputActionValue& Value )
{
	ServerPlayDance ( 1 );
}
void ATTPlayerCharacter::Dance3 ( const FInputActionValue& Value )
{
	ServerPlayDance ( 2 );
}
void ATTPlayerCharacter::Dance4 ( const FInputActionValue& Value )
{
	ServerPlayDance ( 3 );
}
void ATTPlayerCharacter::Dance5 ( const FInputActionValue& Value )
{
	ServerPlayDance ( 4 );
}
void ATTPlayerCharacter::ServerPlayDance_Implementation ( int32 Index )
{
	if (bIsStunned || bIsDead || CurrentComboCount > 0) return;

	MulticastPlayDance ( Index );
}

void ATTPlayerCharacter::MulticastPlayDance_Implementation ( int32 Index )
{
	if (bIsStunned || bIsDead) return;
	UAnimInstance* AnimInstance = GetMesh ()->GetAnimInstance ();
	if (AnimInstance && DanceMontages.IsValidIndex ( Index ) && DanceMontages[Index])
	{
		if (CurrentDanceAudio)
		{
			CurrentDanceAudio->Stop ();
			CurrentDanceAudio = nullptr;
		}

		AnimInstance->Montage_Play ( DanceMontages[Index] );

		SetHoldablesVisible ( false );

		if (DanceSounds.IsValidIndex ( Index ) && DanceSounds[Index])
		{
			CurrentDanceAudio = UGameplayStatics::SpawnSoundAttached ( DanceSounds[Index] , GetRootComponent () );
		}
	}
}
void ATTPlayerCharacter::StopDanceAndMusic()
{
	if (IsLocallyControlled ())
	{
		ServerStopDance ();
	}
	if (HasAuthority ())
	{
		MulticastStopDanceAndMusic ();
	}
}
void ATTPlayerCharacter::MulticastStopDanceAndMusic_Implementation ()
{
	// 1. 애니메이션 중지 로직
	UAnimInstance* AnimInstance = GetMesh ()->GetAnimInstance ();
	if (AnimInstance)
	{
		for (UAnimMontage* Montage : DanceMontages)
		{
			if (AnimInstance->Montage_IsPlaying ( Montage ))
			{
				AnimInstance->Montage_Stop ( 0.25f , Montage );
			}
		}
	}
	SetHoldablesVisible ( true );
	if (CurrentDanceAudio)
	{
		CurrentDanceAudio->Stop ();
		CurrentDanceAudio->DestroyComponent ();
		CurrentDanceAudio = nullptr;
	}
}
void ATTPlayerCharacter::ServerStopDance_Implementation ()
{
	MulticastStopDanceAndMusic ();
}
void ATTPlayerCharacter::SetHoldablesVisible ( bool bVisible )
{
	if (CurrentSword) CurrentSword->SetActorHiddenInGame ( !bVisible );
	if (CurrentSword02) CurrentSword02->SetActorHiddenInGame ( !bVisible );
	if (CurrentAxe) CurrentAxe->SetActorHiddenInGame ( !bVisible );
	if (CurrentHammer) CurrentHammer->SetActorHiddenInGame ( !bVisible );
	if (CurrentShield) CurrentShield->SetActorHiddenInGame ( !bVisible );
	if (CurrentShield02) CurrentShield02->SetActorHiddenInGame ( !bVisible );
}
void ATTPlayerCharacter::PickUp ( const FInputActionValue& Value )
{
	ServerPickUp ();
}

void ATTPlayerCharacter::ThrowAway ( const FInputActionValue& Value )
{
	if (IsHoldingAnything ())
	{
		ServerThrowAway ();
	}
}

void ATTPlayerCharacter::ServerSetBlocking_Implementation ( bool bNewBlocking )
{
	bIsBlocking = bNewBlocking;
}

void ATTPlayerCharacter::ServerThrowAway_Implementation ()
{
	bool bHasWeapon = false;

	if (IsValid ( CurrentSword )) { CurrentSword->HandleOnThrowAway ();   CurrentSword = nullptr;   bHasWeapon = true; }
	if (IsValid ( CurrentSword02 )) { CurrentSword02->HandleOnThrowAway (); CurrentSword02 = nullptr; bHasWeapon = true; }
	if (IsValid ( CurrentAxe )) { CurrentAxe->HandleOnThrowAway ();     CurrentAxe = nullptr;     bHasWeapon = true; }
	if (IsValid ( CurrentHammer )) { CurrentHammer->HandleOnThrowAway ();  CurrentHammer = nullptr;  bHasWeapon = true; }

	if (bHasWeapon)
	{
		SetWeaponData ( FName ( "Hand" ) );
	}

	if (IsValid ( CurrentShield )) { CurrentShield->HandleOnThrowAway ();   CurrentShield = nullptr; }
	if (IsValid ( CurrentShield02 )) { CurrentShield02->HandleOnThrowAway (); CurrentShield02 = nullptr; }
}

bool ATTPlayerCharacter::HasAnyWeapon () const
{
	return IsValid ( CurrentSword ) || IsValid ( CurrentAxe ) || IsValid ( CurrentHammer );
}

void ATTPlayerCharacter::ServerPickUp_Implementation ()
{
	if (!IsValid ( OverlappingPickupComponent )) return;

	AActor* PickedActor = OverlappingPickupComponent->GetOwner ();
	if (!PickedActor) return;

	bool bIsNewWeapon = PickedActor->IsA ( ATTSword::StaticClass () ) || PickedActor->IsA ( ATTSword02::StaticClass () ) ||
		PickedActor->IsA ( ATTAxe::StaticClass () ) || PickedActor->IsA ( ATTHammer::StaticClass () );

	bool bIsNewShield = PickedActor->IsA ( ATTShield::StaticClass () ) || PickedActor->IsA ( ATTShield02::StaticClass () );

	if (bIsNewWeapon)
	{
		if (IsValid ( CurrentSword )) { CurrentSword->HandleOnThrowAway ();   CurrentSword = nullptr; }
		if (IsValid ( CurrentSword02 )) { CurrentSword02->HandleOnThrowAway (); CurrentSword02 = nullptr; }
		if (IsValid ( CurrentAxe )) { CurrentAxe->HandleOnThrowAway ();     CurrentAxe = nullptr; }
		if (IsValid ( CurrentHammer )) { CurrentHammer->HandleOnThrowAway ();  CurrentHammer = nullptr; }

		if (ATTSword* NewSword = Cast<ATTSword> ( PickedActor )) { CurrentSword = NewSword; SetWeaponData ( NewSword->WeaponRowName ); }
		else if (ATTSword02* NewSw02 = Cast<ATTSword02> ( PickedActor )) { CurrentSword02 = NewSw02; SetWeaponData ( NewSw02->WeaponRowName ); }
		else if (ATTAxe* NewAxe = Cast<ATTAxe> ( PickedActor )) { CurrentAxe = NewAxe; SetWeaponData ( NewAxe->WeaponRowName ); }
		else if (ATTHammer* NewHam = Cast<ATTHammer> ( PickedActor )) { CurrentHammer = NewHam; SetWeaponData ( NewHam->WeaponRowName ); }
	}
	else if (bIsNewShield)
	{
		if (IsValid ( CurrentShield )) { CurrentShield->HandleOnThrowAway ();   CurrentShield = nullptr; }
		if (IsValid ( CurrentShield02 )) { CurrentShield02->HandleOnThrowAway (); CurrentShield02 = nullptr; }

		if (ATTShield* NewSh = Cast<ATTShield> ( PickedActor )) { CurrentShield = NewSh; }
		else if (ATTShield02* NewSh02 = Cast<ATTShield02> ( PickedActor )) { CurrentShield02 = NewSh02; }
	}

	OverlappingPickupComponent->ForcePickUp ( this );
}
bool ATTPlayerCharacter::IsHoldingWeapon () const
{
	// 공격 무기
	return (IsValid ( CurrentSword ) || IsValid ( CurrentSword02 ) || IsValid ( CurrentAxe ) || IsValid ( CurrentHammer ));
}

bool ATTPlayerCharacter::IsHoldingShield () const
{
	// 방패
	return (IsValid ( CurrentShield ) || IsValid ( CurrentShield02 ));
}
bool ATTPlayerCharacter::IsHoldingAnything () const
{
	return (IsValid ( CurrentSword ) || IsValid ( CurrentSword02 ) || IsValid ( CurrentAxe ) || IsValid ( CurrentHammer ) || IsValid ( CurrentShield ) || IsValid ( CurrentShield02 ));
}

void ATTPlayerCharacter::OnAnimation ()
{
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetController () ))
	{
		PC->OnAnimation ();
	}
}

void ATTPlayerCharacter::EndAnimation ()
{
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetController () ))
	{
		PC->EndAnimation ();
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
	SetSprintSpeed ( false );
}

#pragma endregion

#pragma region MeshChange

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


#pragma region Attack


void ATTPlayerCharacter::Attack ( const FInputActionValue& Value )
{
	StopDanceAndMusic ();
	if (bIsStunned) return;
	if (GetCharacterMovement ()->IsFalling () == true)
	{
		return;
	}

	ServerStartAttack ();

	if (IsValid ( WeaponData ))
	{
		FTTWeaponData* CurrentWeapon = WeaponData->FindRow<FTTWeaponData> ( WeaponName , TEXT ( "WeaponError" ) );
		if (CurrentWeapon != nullptr)
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "Current StunAmount is %f" ) , CurrentWeapon->StunAmount );
		}
	}

}
void ATTPlayerCharacter::ServerStartAttack_Implementation ()
{
	if (bIsStunned) return;
	if (GetCharacterMovement ()->IsFalling ()) return;

	if (CurrentComboCount != 0)
	{
		bIsAttackKeyPressed = true;
		return;
	}

	CurrentComboCount = 1;
	bIsAttackKeyPressed = false;
	bHasHitThisCombo = false;
	MulticastPlayAttackMontage ( CurrentComboCount );
}
void ATTPlayerCharacter::MulticastPlayAttackMontage_Implementation ( int32 ComboIndex )
{
	if (bIsStunned || bIsDead)
		return;

	UTTAnimInstance* AnimInstance =
		Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );

	if (!AnimInstance || !AttackMeleeMontage)
		return;

	// 첫 공격일 때만 Play
	if (!AnimInstance->Montage_IsPlaying ( AttackMeleeMontage ))
	{
		AnimInstance->Montage_Play ( AttackMeleeMontage );

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject ( this , &ATTPlayerCharacter::EndAttack );
		AnimInstance->Montage_SetEndDelegate ( EndDelegate , AttackMeleeMontage );
	}

	// 콤보 섹션 이동
	const FName SectionName =
		*FString::Printf ( TEXT ( "%s%02d" ) ,
			*AttackAnimMontageSectionPrefix ,
			ComboIndex );

	AnimInstance->Montage_JumpToSection ( SectionName , AttackMeleeMontage );
}

void ATTPlayerCharacter::HandleOnCheckHit ()
{
	if (!IsLocallyControlled ()) return;
	ServerHandleOnCheckHit ();



}
void ATTPlayerCharacter::ServerHandleOnCheckHit_Implementation ()
{
	if (!HasAuthority ()) return;
	if (bHasHitThisCombo) return;
	bHasHitThisCombo = true;
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params ( NAME_None , false , this );

	bool bResult = GetWorld ()->SweepMultiByChannel (
		HitResults ,
		GetActorLocation () ,
		GetActorLocation () + AttackMeleeRange * GetActorForwardVector () ,
		FQuat::Identity ,
		ECC_ATTACK ,
		FCollisionShape::MakeSphere ( AttackMeleeRadius ) ,
		Params
	);
	TSet<AActor*> DamagedActors;

	if (!bResult) return;

	float CurrentStunPower = 0.f;

	if (WeaponData)
	{
		if (FTTWeaponData* WeaponRow =
			WeaponData->FindRow<FTTWeaponData> ( WeaponName , TEXT ( "CheckHit" ) ))
		{
			CurrentStunPower = WeaponRow->StunAmount;
		}
	}

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor ();

		if (!IsValid ( HitActor )) continue;
		if (HitActor == this) continue;

		// ❗ 이미 때린 액터면 스킵
		if (DamagedActors.Contains ( HitActor )) continue;

		DamagedActors.Add ( HitActor );

		FDamageEvent DamageEvent;
		HitActor->TakeDamage ( CurrentStunPower , DamageEvent , GetController () , this );
	}
}

void ATTPlayerCharacter::HandleOnCheckInputAttack ()
{
	if (!HasAuthority ())
	{
		ServerRequestNextCombo ();
		return;
	}

	ServerRequestNextCombo_Implementation ();

}
void ATTPlayerCharacter::ServerRequestNextCombo_Implementation ()
{
	if (!bIsAttackKeyPressed)
		return;

	if (CurrentComboCount >= MaxComboCount)
		return;

	CurrentComboCount++;
	bIsAttackKeyPressed = false;
	bHasHitThisCombo = false;
	MulticastPlayAttackMontage ( CurrentComboCount );
}

void ATTPlayerCharacter::EndAttack ( UAnimMontage* InMontage , bool bInterruped )
{
	if (!HasAuthority ()) return;

	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bHasHitThisCombo = false;
}

float ATTPlayerCharacter::TakeDamage ( float DamageAmount , FDamageEvent const& DamageEvent , AController* EventInstigator , AActor* DamageCauser )
{
	const UDamageType* DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType> () : nullptr;
	bool bIsGasDamage = (DamageType && DamageType->IsA ( UGas_Damage::StaticClass () ));

	if (!bIsGasDamage && bIsBlocking && IsValid(DamageCauser))
	{
		FVector MyForward = GetActorForwardVector();
		FVector ToAttacker = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		float DotResult = FVector::DotProduct(MyForward, ToAttacker);

		if (DotResult > 0.5f)
		{
			if (ShieldBlockSound)
			{
				UGameplayStatics::PlaySoundAtLocation ( this , ShieldBlockSound , GetActorLocation () );
			}
			return 0.0f;
		}
	}

	Super::TakeDamage ( DamageAmount , DamageEvent , EventInstigator , DamageCauser );

	// 피해자쪽 로직.

	const float FinalDamageAmount = DamageAmount;

	if (DamageType && DamageType->IsA ( UGas_Damage::StaticClass () ))
	{
		CurrentHP = FMath::Clamp ( CurrentHP - FinalDamageAmount, 0.f , MaxHP );
	}

	else
	{
		if (!bIsInvincibility)
		{
			CurrentStun = FMath::Clamp ( CurrentStun + FinalDamageAmount , 0.f , MaxStun );
		}
		else
		{
			return 0.0f;
		}
	}

	UE_LOG ( LogTemp , Warning ,TEXT ( "[%s] HP: %f / %f | Stun: %f / %f" ) ,*GetName () , CurrentHP , MaxHP , CurrentStun , MaxStun);

	if (FinalDamageAmount> 0.f && !bIsStunned && !bIsDead)
	{
		StopDanceAndMusic ();
		MulticastPlayHitMontage ();
	}

	if (CurrentHP <= 0.f && !bIsDead)
	{
		if (HasAuthority ())
		{
			ServerDeath ();
		}
	}

	if (CurrentStun >= MaxStun && !bIsStunned)
	{
		KnockOut ();
	}

	return FinalDamageAmount;
}

void ATTPlayerCharacter::SetWeaponData ( FName NewWeaponName )
{
	WeaponName = NewWeaponName;
	UE_LOG ( LogTemp , Warning , TEXT ( "Weapon Changed to : %s" ) , *WeaponName.ToString () );
}

void ATTPlayerCharacter::KnockOut ()
{
	if (bIsStunned) return;
	UE_LOG ( LogTemp , Warning , TEXT ( "%s is KNOCKED OUT!" ) , *GetName () );

	SetInvincibility ( true );
	bIsStunned = true;
	CurrentStun = 0.0f;

	OnRep_IsStunned ();

	GetWorld ()->GetTimerManager ().SetTimer (
		StunTimerHandle ,
		this ,
		&ATTPlayerCharacter::WakeUp ,
		StunDuration ,
		false
	);
}

void ATTPlayerCharacter::OnRep_IsStunned ()
{
	if (bIsStunned)
	{
		GetCharacterMovement ()->SetMovementMode ( EMovementMode::MOVE_None );
		GetCapsuleComponent ()->SetCollisionEnabled ( ECollisionEnabled::NoCollision );

		GetMesh ()->SetCollisionProfileName ( TEXT ( "TT_Ragdoll" ) );
		GetMesh ()->SetSimulatePhysics ( true );

		SpringArm->AttachToComponent ( GetMesh () , FAttachmentTransformRules::KeepWorldTransform , TEXT ( "pelvis" ) );
	}
	else
	{
		FVector RagdollLoc = GetMesh ()->GetSocketLocation ( TEXT ( "pelvis" ) );
		float CapsuleHalfHeight = GetCapsuleComponent ()->GetScaledCapsuleHalfHeight ();

		FVector TargetLoc = FVector ( RagdollLoc.X , RagdollLoc.Y , RagdollLoc.Z + CapsuleHalfHeight + 20.0f );

		SetActorLocation ( TargetLoc , false , nullptr , ETeleportType::TeleportPhysics );

		GetMesh ()->SetSimulatePhysics ( false );
		GetMesh ()->SetCollisionProfileName ( TEXT ( "CharacterMesh" ) );

		GetMesh ()->AttachToComponent ( GetCapsuleComponent () , FAttachmentTransformRules::SnapToTargetNotIncludingScale );
		GetMesh ()->SetRelativeLocation ( FVector ( 0.0f , 0.0f , -60.0f ) );
		GetMesh ()->SetRelativeRotation ( FRotator ( 0.0f , -90.0f , 0.0f ) );

		SpringArm->AttachToComponent ( GetCapsuleComponent () , FAttachmentTransformRules::SnapToTargetNotIncludingScale );
		SpringArm->SetRelativeLocation ( FVector ( 0.0f , 0.0f , 0.0f ) );

		GetCapsuleComponent ()->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
		GetCharacterMovement ()->SetMovementMode ( EMovementMode::MOVE_Walking );

		GetMesh ()->UpdateBounds ();
		GetMesh ()->RefreshBoneTransforms ();
	}
}

void ATTPlayerCharacter::WakeUp ()
{
	UE_LOG ( LogTemp , Warning , TEXT ( "%s Woke Up!" ) , *GetName () );

	SetInvincibility ( false );
	bIsStunned = false;

	OnRep_IsStunned ();
}

void ATTPlayerCharacter::OnRep_ServerRagdollLocation ()
{
	if (!bIsStunned || bIsDead ==0 || HasAuthority ()) return;

	FVector CurrentLoc = GetMesh ()->GetSocketLocation ( TEXT ( "pelvis" ) );

	float Dist = FVector::Dist ( CurrentLoc , ServerRagdollLocation );

	if (Dist > 100.0f)
	{
		GetMesh ()->SetWorldLocation ( ServerRagdollLocation , false , nullptr , ETeleportType::TeleportPhysics );

		GetMesh ()->SetWorldRotation ( ServerRagdollRotation , false , nullptr , ETeleportType::TeleportPhysics );
		GetMesh ()->SetPhysicsLinearVelocity ( ServerRagdollVelocity );
		GetMesh ()->SetPhysicsAngularVelocityInDegrees ( ServerRagdollAngularVelocity );
	}
	else if (Dist > 10.0f)
	{
		FVector FixDirection = (ServerRagdollLocation - CurrentLoc);

		float CorrectionPower = 10.0f;

		FVector NewVelocity = ServerRagdollVelocity + (FixDirection * CorrectionPower);

		GetMesh ()->SetPhysicsLinearVelocity ( NewVelocity );
	}
}

void ATTPlayerCharacter::MulticastPlayHitMontage_Implementation ()
{
	if (bIsStunned || bIsDead) return;

	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );

	if (IsValid ( AnimInstance ))
	{
		if (HitMontage)
		{
			AnimInstance->Montage_Play ( HitMontage );
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation ( this , HitSound , GetActorLocation () );
		}
	}
}
void ATTPlayerCharacter::ApplySlow ( float Amount , float Duration )
{
	if (!HasAuthority ()) return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement ();
	if (!MoveComp) return;

	GetWorldTimerManager ().ClearTimer ( SlowTimerHandle );

	MoveComp->MaxWalkSpeed = BaseWalkSpeed * (1.f - Amount);

	GetWorldTimerManager ().SetTimer (
		SlowTimerHandle ,
		this ,
		&ATTPlayerCharacter::ClearSlow ,
		Duration ,
		false
	);
}

void ATTPlayerCharacter::ApplyStun ( float Amount )
{
	if (!HasAuthority ()) return;

	CurrentStun = FMath::Clamp ( CurrentStun + Amount , 0.f , MaxStun );

	UE_LOG ( LogTemp , Warning ,
		TEXT ( "[%s] Stun: %f / %f" ) ,
		*GetName () , CurrentStun , MaxStun
	);

	if (CurrentStun >= MaxStun && !bIsStunned)
	{
		KnockOut ();
	}
}

void ATTPlayerCharacter::AddThrowable ( AThrowableBase* Throwable )
{
	if (!Throwable) return;

	CurrentThrowable = Throwable;
}

void ATTPlayerCharacter::OnRep_IsDead ()
{
	if (bIsDead == 1)
	{
		if (CurrentSword) ServerThrowAway ();
		if (CurrentShield) ServerThrowAway ();

		GetMesh ()->SetCollisionProfileName ( TEXT ( "Ragdoll" ) );
		GetMesh ()->SetSimulatePhysics ( true );

		GetCapsuleComponent ()->SetCollisionEnabled ( ECollisionEnabled::NoCollision );

		GetCharacterMovement ()->StopMovementImmediately ();
		GetCharacterMovement ()->DisableMovement ();

		if (OnPlayerDied.IsBound ())
		{
			OnPlayerDied.Broadcast ();
		}
	}
}

void ATTPlayerCharacter::ServerDeath_Implementation ()
{
	if (bIsDead != 0) return;

	bIsDead = 1;
	AInGameModeBase* TTGM = Cast<AInGameModeBase> ( GetWorld ()->GetAuthGameMode () );
	if (IsValid ( TTGM ))
	{
		if(ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
		{
			if(PS->GetTeam() == Teams::Blue)
			{
				TTGM->SetBlueTeamCount ();
			}
			if(PS->GetTeam() == Teams::Red)
			{
				TTGM->SetRedTeamCount ();
			}
		}
	}
	OnRep_IsDead ();

	if (GetWorld ())
	{
		GetWorld ()->GetTimerManager ().SetTimer
		(
			DeathTimerHandle ,
			this ,
			&ATTPlayerCharacter::StartGhost ,
			SpectateDelayTime ,
			false
		);
	}
}

void ATTPlayerCharacter::ClearSlow ()
{
	GetCharacterMovement ()->MaxWalkSpeed = BaseWalkSpeed;
}

#pragma endregion
