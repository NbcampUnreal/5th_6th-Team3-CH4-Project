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
	if (HasAuthority ())
	{
		if(ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
		{
			if (USkeletalMesh* HeadMesh = PS->PersistedHeadMesh)
			{
				OnRep_HeadMesh ();
				HeadMeshToReplicate = HeadMesh;
			}
			if (USkeletalMesh* BodyMesh = PS->PersistedBodyMesh)
			{
				OnRep_BodyMesh ();
				BodyMeshToReplicate = BodyMesh;
			}
		}
	}
	LoadPlayerSaveData ( TEXT ( "MySaveSlot_01" ) , 0 );
}

void ATTPlayerCharacter::Tick ( float DeltaTime )
{
	Super::Tick ( DeltaTime );

	float TurnSpeed = GetCharacterMovement ()->RotationRate.Yaw;
	FRotator NewRotation = FMath::RInterpConstantTo ( GetActorRotation () , TargetRotation , DeltaTime , TurnSpeed );

	SetActorRotation ( NewRotation );
}

void ATTPlayerCharacter::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( ATTPlayerCharacter , HeadMeshToReplicate );
	DOREPLIFETIME ( ATTPlayerCharacter , BodyMeshToReplicate );

	DOREPLIFETIME ( ATTPlayerCharacter , TargetRotation );
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
			TargetRotation = DesiredDirection.Rotation ();

			ServerSetRotation ( TargetRotation );
		}

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
	SetSprintSpeed ( true );
	ServerSprintStart ();
}
void ATTPlayerCharacter::SprintEnd ()
{
	SetSprintSpeed ( false );
	ServerSprintEnd ();
}

void ATTPlayerCharacter::PlayerBlocking ()
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
	if(ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
	{
		PS->PersistedHeadMesh = NewMesh;
	}

	HeadMeshToReplicate = NewMesh;
	OnRep_HeadMesh ();
}

bool ATTPlayerCharacter::ServerChangeBodyMesh_Validate ( USkeletalMesh* NewMesh )
{
	return true;
}

void ATTPlayerCharacter::ServerChangeBodyMesh_Implementation ( USkeletalMesh* NewMesh )
{
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
	{
		PS->PersistedBodyMesh = NewMesh;
	}

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
		SavePlayerSaveData ( TEXT ( "MySaveSlot_01" ) , 0 );
	}
}

void ATTPlayerCharacter::ChangeBody ( USkeletalMesh* NewMesh )
{
	if (IsValid ( GetMesh () ) && IsValid ( NewMesh ))
	{
		GetMesh ()->SetSkeletalMesh ( NewMesh );
		SavePlayerSaveData ( TEXT ( "MySaveSlot_01" ) , 0 );
	}
}

#pragma endregion

#pragma region SaveData

void ATTPlayerCharacter::SavePlayerSaveData ( const FString& SlotName , int32 UserIndex )
{
	USkeletalMeshComponent* HeadMesh = Head;
	USkeletalMeshComponent* BodyMesh = GetMesh ();

	if (IsValid ( HeadMesh ) && IsValid ( BodyMesh ))
	{
		USaveGame* SaveGameInstance = UGameplayStatics::CreateSaveGameObject ( USaveGame::StaticClass () );
		if (!SaveGameInstance) 
			return;

		UTTSaveGame* TTSaveGameInstance = Cast<UTTSaveGame> ( SaveGameInstance );
		if (!TTSaveGameInstance) 
			return;
		// 세이브 데이터에 세이브
		if (USkeletalMesh* CurrentHeadMesh = HeadMesh->GetSkeletalMeshAsset ())
		{
			TTSaveGameInstance->CurrentHeadMeshPath = FSoftObjectPath ( CurrentHeadMesh );
		}
		if (USkeletalMesh* CurrentBodyMesh = BodyMesh->GetSkeletalMeshAsset ())
		{
			TTSaveGameInstance->CurrentBodyMeshPath = FSoftObjectPath ( CurrentBodyMesh );
		}
		UGameplayStatics::SaveGameToSlot ( TTSaveGameInstance , SlotName , UserIndex );
	}
}

void ATTPlayerCharacter::LoadPlayerSaveData ( const FString& SlotName , int32 UserIndex )
{
	if(IsValid( Head ) && IsValid ( GetMesh () ))
	{
		// 세이브 데이터를 불러오는 과정
		USaveGame* LoadGameInstance = UGameplayStatics::CreateSaveGameObject ( USaveGame::StaticClass () );
		if (!LoadGameInstance)
			return;

		UTTSaveGame* TTLoadGameInstance = Cast<UTTSaveGame> ( LoadGameInstance );
		if (!TTLoadGameInstance)
			return;
		USkeletalMesh* LoadedHeadMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentHeadMeshPath.ResolveObject () );
		if (!TTLoadGameInstance->CurrentHeadMeshPath.IsNull () && !TTLoadGameInstance->CurrentBodyMeshPath.IsNull())
			return;

		// 로드된 세이브 데이터를 불러오는 과정 
		USkeletalMesh* LoadHeadMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentHeadMeshPath.ResolveObject () );
		USkeletalMesh* LoadBodyMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentBodyMeshPath.ResolveObject () );

		// 세이브 데이터 적용
		if (IsValid ( LoadHeadMesh ) && IsValid ( LoadBodyMesh ))
		{
			Head->SetSkeletalMesh ( LoadHeadMesh );
			GetMesh ()->SetSkeletalMesh ( LoadBodyMesh );
		}
	}
}
#pragma endregion
