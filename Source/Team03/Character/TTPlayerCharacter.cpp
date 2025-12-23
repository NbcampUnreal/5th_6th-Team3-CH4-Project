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
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/EngineTypes.h"

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
	bIsStunned(false)
{
	WeaponName = "Hand";
	WeaponData = nullptr;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D> ( TEXT ( "SceneCapture" ) );
	SceneCapture->TextureTarget = CaptureRT;
	SceneCapture->SetupAttachment ( GetRootComponent() );

	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->PrimitiveRenderMode =
		ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

	SceneCapture->ShowOnlyComponents.Add ( GetMesh () );
	SceneCapture->FOVAngle = 35.f;

	SceneCapture->bEnableClipPlane = true;
	SceneCapture->CaptureScene();

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
		PlayerController->SetControlRotation (FRotator(0.f , -70.f , 0.f ));
		PlayerController->PlayerCameraManager->ViewPitchMin = -80.f ;
		PlayerController->PlayerCameraManager->ViewPitchMax = -30.f ;


		// ----- Outgame 담당자가 수정함 -----
		/* 
		 * LoadPlayerSaveData 호출 비활성화:
		 * - SaveGame 로드가 PlayerState 데이터를 덮어쓰는 문제 발생
		 * - Seamless Travel을 사용하므로 PlayerState가 자동으로 유지됨
		 * PlayerController->LoadPlayerSaveData ( TEXT ( "MySaveSlot_01" ) , 0 );
		 */
		// ----------------------------------


		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> ( PlayerController->GetLocalPlayer() );

		if (IsValid ( Subsystem ) == true)
		{
			Subsystem->AddMappingContext ( IMC_Character , 0 );
		}
	}
	// ----- Outgame 담당자가 수정함 -----
	if (HasAuthority ())
	{
		if(ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
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
	if (bIsStunned && HasAuthority ())
	{
		ServerRagdollLocation = GetMesh ()->GetSocketLocation ( TEXT ( "pelvis" ) );
		ServerRagdollRotation = GetMesh ()->GetSocketRotation ( TEXT ( "pelvis" ) );
		ServerRagdollVelocity = GetMesh ()->GetPhysicsLinearVelocity ( TEXT ( "pelvis" ) );
		ServerRagdollAngularVelocity = GetMesh ()->GetPhysicsAngularVelocityInDegrees ( TEXT ( "pelvis" ) );
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

		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::JumpStart );
		EnhancedInputComponent->BindAction ( InputJump , ETriggerEvent::Completed , this , &ATTPlayerCharacter::JumpEnd );

		EnhancedInputComponent->BindAction ( InputEnter , ETriggerEvent::Started , this , &ATTPlayerCharacter::InChat );
		EnhancedInputComponent->BindAction ( InputESC , ETriggerEvent::Started , this , &ATTPlayerCharacter::ESCMenu );
		EnhancedInputComponent->BindAction ( InputTempKey , ETriggerEvent::Started , this , &ATTPlayerCharacter::TempKey );

		EnhancedInputComponent->BindAction ( InputTempKey , ETriggerEvent::Triggered , this , &ATTPlayerCharacter::PickUpStart );
		EnhancedInputComponent->BindAction ( InputTempKey , ETriggerEvent::Completed , this , &ATTPlayerCharacter::PickUpEnd );

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
	if (bIsStunned) return;
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

void ATTPlayerCharacter::JumpStart ()
{
	if (bIsStunned) return;

	Super::Jump ();
}

void ATTPlayerCharacter::JumpEnd ()
{
	if (bIsStunned) return;

	Super::StopJumping ();
}

void ATTPlayerCharacter::PickUpStart ()
{
}

void ATTPlayerCharacter::PickUpEnd ()
{
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
	if (bIsStunned) return;
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

	if (IsValid ( WeaponData ))
	{
		FTTWeaponData* CurrentWeapon = WeaponData->FindRow<FTTWeaponData> ( WeaponName , TEXT ( "WeaponError" ) );
		if (CurrentWeapon != nullptr)
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "Current StunAmount is %f" ) , CurrentWeapon->StunAmount );
		}
	}

}

void ATTPlayerCharacter::HandleOnCheckHit ()
{
	if (HasAuthority ())
	{
		ServerHandleOnCheckHit_Implementation ();
	}
	else
	{
		ServerHandleOnCheckHit ();
	}

}
void ATTPlayerCharacter::ServerHandleOnCheckHit_Implementation ()
{
	if (!HasAuthority ()) return;

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

	if (HitResults.IsEmpty () == false)
	{
		float CurrentStunPower = 0.0f;
		float CurrentKnockback = 0.0f;

		if (IsValid ( WeaponData ))
		{
			FTTWeaponData* WeaponRow = WeaponData->FindRow<FTTWeaponData> ( WeaponName , TEXT ( "CheckHit" ) );
			if (WeaponRow)
			{
				CurrentStunPower = WeaponRow->StunAmount;
				CurrentKnockback = WeaponRow->KnockbackAmount;
			}
		}

		for (FHitResult HitResult : HitResults)
		{

			if (IsValid ( HitResult.GetActor () ) == true)
			{
				FDamageEvent DamageEvent;
				HitResult.GetActor ()->TakeDamage ( CurrentStunPower , DamageEvent , GetController () , this );
			}
		}
	}
}

void ATTPlayerCharacter::HandleOnCheckInputAttack ()
{
	//UKismetSystemLibrary::PrintString ( this , TEXT ( "HandleOnCheckInputAttack()" ) );
	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	checkf ( IsValid ( AnimInstance ) == true , TEXT ( "Invalid AnimInstance" ) );

	if (bIsAttackKeyPressed == true)
	{
		// 현재 콤보가 MaxComboCount보다 작을 때만 다음으로 진행
		if (CurrentComboCount < MaxComboCount)
		{
			CurrentComboCount++;

			FName NextSectionName = *FString::Printf ( TEXT ( "%s%02d" ) , *AttackAnimMontageSectionPrefix , CurrentComboCount );
			AnimInstance->Montage_JumpToSection ( NextSectionName , AttackMeleeMontage );
		}

		bIsAttackKeyPressed = false;
	}

}
void ATTPlayerCharacter::BeginAttack ()
{
	UTTAnimInstance* AnimInstance = Cast<UTTAnimInstance> ( GetMesh ()->GetAnimInstance () );
	checkf ( IsValid ( AnimInstance ) == true , TEXT ( "Invalid AnimInstance" ) );
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

float ATTPlayerCharacter::TakeDamage ( float DamageAmount , FDamageEvent const& DamageEvent , AController* EventInstigator , AActor* DamageCauser )
{
	float FinalDamageAmount = Super::TakeDamage ( DamageAmount , DamageEvent , EventInstigator , DamageCauser );
	// 피해자쪽 로직.

	CurrentStun = FMath::Clamp ( CurrentStun + FinalDamageAmount , 0.0f , MaxStun );

	UE_LOG ( LogTemp , Warning , TEXT ( "[%s] Current Stun : %f / %f" ) , *GetName () , CurrentStun , MaxStun );
	if (FinalDamageAmount > 0.0f && !bIsStunned)
	{
		MulticastPlayHitMontage ();
	}
	if (CurrentStun >= MaxStun)
	{
		if (bIsStunned == false)
		{
			KnockOut ();
		}
	}
	//if (1 == ShowAttackMeleeDebug)
	//{
	//	UKismetSystemLibrary::PrintString ( this , FString::Printf ( TEXT ( "%s was taken damage: %.3f" ) , *GetName () , FinalDamageAmount ) );
	//}

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

		FVector TargetLoc = FVector ( RagdollLoc.X , RagdollLoc.Y , RagdollLoc.Z + CapsuleHalfHeight +20.0f);

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

	bIsStunned = false;

	OnRep_IsStunned ();
}
void ATTPlayerCharacter::OnRep_ServerRagdollLocation ()
{
	if (!bIsStunned || HasAuthority ()) return;

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

	// 🔧 변경: 중첩 방지
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

void ATTPlayerCharacter::ClearSlow ()
{
	GetCharacterMovement ()->MaxWalkSpeed = BaseWalkSpeed;
}

#pragma endregion
