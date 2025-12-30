#include "TTPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InGameMode/InGameModeBase.h"
#include "InGameUI/TTInGameHUD.h"
#include "InGameUI/TTChatUI.h"
#include "Character/TTPlayerCharacter.h"
#include "Outgame/TTGameInstance.h"
#include "Character/TTPlayerState.h"
#include "Save/TTSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "InGameUI/TTPlayerPortraitWidget.h"

ATTPlayerController::ATTPlayerController ()
	: InputMappingContext ( nullptr ) ,
	MoveAction ( nullptr ) ,
	JumpAction ( nullptr ) ,
	SprintAction ( nullptr ) ,
	AttackAction ( nullptr ) ,
	BlockingAction ( nullptr ),
	Dance1Action ( nullptr ),
	Dance2Action ( nullptr ) ,
	Dance3Action ( nullptr ) ,
	Dance4Action ( nullptr ) ,
	Dance5Action ( nullptr )
{
	bReplicates = true;
}

void ATTPlayerController::SetPawn ( APawn* InPawn )
{
	Super::SetPawn ( InPawn );
	
	// ----- Outgame 담당자가 수정함 -----
	if (InPawn)
	{
		ATTPlayerCharacter* NewChar = Cast<ATTPlayerCharacter> ( InPawn );
		ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ();

		if (NewChar && PS)
		{
			NewChar->InitializeMesh ( PS );
		}
	}
}

void ATTPlayerController::OnPossess ( APawn* InPawn )
{
	Super::OnPossess ( InPawn );

	// ----- Outgame 담당자가 수정함 -----
	if (InPawn)
	{
		ATTPlayerCharacter* NewChar = Cast<ATTPlayerCharacter> ( InPawn );
		ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ();

		if (NewChar && PS)
		{
			NewChar->InitializeMesh ( PS );
		}
	}

	if (IsLocalController ())
	{
		PlayerSetUp ();
	}
}

void ATTPlayerController::BeginPlay ()
{
	Super::BeginPlay ();

	// 현재 PlayerController에 연결된 Local Player 객체를 가져옴    
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer ())
	{
		// Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem> ())
		{
			if (InputMappingContext)
			{
				// Subsystem을 통해 우리가 할당한 IMC를 활성화
				// 우선순위(Priority)는 0이 가장 높은 우선순위
				Subsystem->AddMappingContext ( InputMappingContext , 0 );
			}
		}
	}

	TTInGameHUD = Cast<ATTInGameHUD> ( GetHUD () );
	if (TTInGameHUD)
	{
		TTInGameHUD->AddChat ();
		TTInGameHUD->AddNotification ();
	}

// ---------- Outgame 담당자가 수정함 ----------
	if (IsLocalController())
	{
		// Force Game Input Mode
		FInputModeGameOnly GameInputMode;
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;

		if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
		{
			int32 HeadIndex = GI->CustomizedHeadIndex;
			int32 BodyIndex = GI->CustomizedBodyIndex;
			
			ServerRPC_InitPlayerInfo(GI->UserNickname, GI->SelectedCharacterRowName, HeadIndex, BodyIndex);
		}
		// ---------- Ingame 담당자가 추가 ----------
		UE_LOG ( LogTemp , Warning , TEXT ( "ServerAddportrait0" ));
		ServerClientReady ();
	}
}

void ATTPlayerController::OnRep_Pawn ()
{
	Super::OnRep_Pawn ();

	PlayerSetUp ();
}

void ATTPlayerController::PlayerSetUp ()
{
	APawn* CurrentPawn = GetPawn ();
	if (!CurrentPawn) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> ( GetLocalPlayer () ))
	{
		Subsystem->ClearAllMappings ();
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext ( InputMappingContext , 0 );
		}
	}

	if (PlayerCameraManager)
	{
		if (!CurrentPawn->IsA ( ATTPlayerCharacter::StaticClass () ))
		{
			PlayerCameraManager->ViewPitchMin = -90.f;
			PlayerCameraManager->ViewPitchMax = 90.f;
		}
		else
		{
			PlayerCameraManager->ViewPitchMin = -80.f;
			PlayerCameraManager->ViewPitchMax = -30.f;
		}
	}
}

// ----- Outgame 담당자가 수정함 -----
void ATTPlayerController::ServerRPC_InitPlayerInfo_Implementation(const FString& Nickname, const FName& CharacterRowName, int32 HeadIndex, int32 BodyIndex)
{
	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		PS->UserNickname = Nickname;
		PS->SelectedCharacterRowName = CharacterRowName;

		// Force update if needed, but replication will handle it
		PS->ForceNetUpdate();
		
		// Load and apply Head Mesh
		const UTTCharactorHeadSkeletalSelect* HeadCDO = GetDefault<UTTCharactorHeadSkeletalSelect>();
		if (HeadCDO && HeadIndex >= 0 && HeadCDO->PlayerCharacterHeadSkeletalPaths.IsValidIndex(HeadIndex))
		{
			FSoftObjectPath HeadPath = HeadCDO->PlayerCharacterHeadSkeletalPaths[HeadIndex];
			if (USkeletalMesh* HeadMesh = Cast<USkeletalMesh>(HeadPath.TryLoad()))
			{
				PS->PersistedHeadMesh = HeadMesh;
			}
		}

		// Load and apply Body Mesh
		const UTTCharactorSkeletalMeshSelect* BodyCDO = GetDefault<UTTCharactorSkeletalMeshSelect>();
		if (BodyCDO && BodyIndex >= 0 && BodyCDO->PlayerCharacterSkeletalPaths.IsValidIndex(BodyIndex))
		{
			FSoftObjectPath BodyPath = BodyCDO->PlayerCharacterSkeletalPaths[BodyIndex];
			if (USkeletalMesh* BodyMesh = Cast<USkeletalMesh>(BodyPath.TryLoad()))
			{
				PS->PersistedBodyMesh = BodyMesh;
			}
		}
		
		// Initialize Character Mesh if Pawn exists
		if (ATTPlayerCharacter* MyCharacter = Cast<ATTPlayerCharacter>(GetPawn()))
		{
			MyCharacter->InitializeMesh(PS);
		}
	}
}

bool ATTPlayerController::ServerRPC_InitPlayerInfo_Validate(const FString& Nickname, const FName& CharacterRowName, int32 HeadIndex, int32 BodyIndex)
{
	return true;
}

#pragma region ChatUI

void ATTPlayerController::ActivateChatBox ()
{
	if (IsValid ( TTInGameHUD ) && TTInGameHUD->Chat)
	{
		TTInGameHUD->Chat->ActivateChat();
	}
}

void ATTPlayerController::ServerSendChatMessage_Implementation ( const FString& Message )
{
	if (!HasAuthority ())
		return;

	if (AInGameModeBase* GM = GetWorld ()->GetAuthGameMode<AInGameModeBase> ())
	{
		if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( GetPawn ()->GetPlayerState () ))
		{
			const FString NameMessage = TTPS->UserNickname + TEXT(" : ") + Message;
			GM->SendChatMessage ( NameMessage );
		}
		else
		{
			GM->SendChatMessage ( Message );
		}
	}
}

void ATTPlayerController::ClientAddChatMessage_Implementation ( const FString& Message )
{
	if (IsValid ( TTInGameHUD ))
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "Multicast received on client! Message: %s" ) , *Message );
		TTInGameHUD->AddChatMessage ( Message );
	}
}

#pragma endregion

#pragma region ESCMenu

void ATTPlayerController::ActivateESCMenu ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->AddESCMenu ();
		SetShowMouseCursor (true);
		FInputModeUIOnly InputMode;
		SetInputMode ( InputMode );
	}
}

void ATTPlayerController::ActivateSelectedSkeletalMesh ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->AddSelectedSkeletalMenu ();
		SetShowMouseCursor ( true );
		FInputModeUIOnly InputMode;
		SetInputMode ( InputMode );
	}
	Cast<ATTPlayerController> ( GetOwner () );
}

void ATTPlayerController::ChangeHeadMesh ( USkeletalMesh* NewMesh )
{
	if (ATTPlayerCharacter* PC = Cast<ATTPlayerCharacter> ( GetPawn () ))
	{
		PC->ServerChangeHeadMesh ( NewMesh );
	}
}

void ATTPlayerController::ChangeMesh ( USkeletalMesh* NewMesh )
{
	if (ATTPlayerCharacter* PC = Cast<ATTPlayerCharacter> ( GetPawn () ))
	{
		PC->ServerChangeBodyMesh ( NewMesh );
	}
}

#pragma endregion

#pragma region SaveData

void ATTPlayerController::SavePlayerSaveData ( const FString& SlotName , int32 UserIndex )
{
	ATTPlayerCharacter* TTPC = Cast<ATTPlayerCharacter> ( GetPawn () );
	USkeletalMeshComponent* HeadMesh = TTPC->Head;
	USkeletalMeshComponent* BodyMesh = TTPC->GetMesh ();

	if (IsValid ( HeadMesh ) && IsValid ( BodyMesh ))
	{
		USaveGame* SaveGameInstance = UGameplayStatics::CreateSaveGameObject ( UTTSaveGame::StaticClass () );
		if (!SaveGameInstance)
			return;

		UTTSaveGame* TTSaveGameInstance = Cast<UTTSaveGame> ( SaveGameInstance );
		if (!TTSaveGameInstance)
			return;
		// 세이브 데이터에 세이브
		if (USkeletalMesh* CurrentHeadMesh = HeadMesh->GetSkeletalMeshAsset ())
		{
			TTSaveGameInstance->CurrentHeadMeshPath = FSoftObjectPath ( CurrentHeadMesh );
			if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
			{
				PS->PersistedHeadMesh = CurrentHeadMesh;
			}
		}
		if (USkeletalMesh* CurrentBodyMesh = BodyMesh->GetSkeletalMeshAsset ())
		{
			TTSaveGameInstance->CurrentBodyMeshPath = FSoftObjectPath ( CurrentBodyMesh );
			if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ())
			{
				PS->PersistedBodyMesh = CurrentBodyMesh;
			}
		}

		UGameplayStatics::SaveGameToSlot ( TTSaveGameInstance , SlotName , UserIndex );
	}
}

void ATTPlayerController::LoadPlayerSaveData ( const FString& SlotName , int32 UserIndex )
{
	ATTPlayerCharacter* TTPC = Cast<ATTPlayerCharacter> ( GetPawn () );
	if (IsValid ( TTPC->Head ) && IsValid ( TTPC->GetMesh () ) && IsValid( TTPC ))
	{
		// 세이브 데이터를 불러오는 과정
		USaveGame* LoadGameInstance = UGameplayStatics::LoadGameFromSlot ( SlotName , UserIndex );
		if (!LoadGameInstance)
			return;

		UTTSaveGame* TTLoadGameInstance = Cast<UTTSaveGame> ( LoadGameInstance );

		if (!IsValid(TTLoadGameInstance) &&
			!TTLoadGameInstance->CurrentHeadMeshPath.IsNull () && 
			!TTLoadGameInstance->CurrentBodyMeshPath.IsNull ())
			return;

		// 로드된 세이브 데이터를 불러오는 과정 
		USkeletalMesh* LoadHeadMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentHeadMeshPath.ResolveObject () );
		USkeletalMesh* LoadBodyMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentBodyMeshPath.ResolveObject () );

		// 세이브 데이터 적용
		if (IsValid ( LoadHeadMesh ) && IsValid ( LoadBodyMesh ))
		{
			TTPC->Head->SetSkeletalMesh ( LoadHeadMesh );
			TTPC->GetMesh ()->SetSkeletalMesh ( LoadBodyMesh );
		}
	}
}
#pragma endregion

#pragma region Notification

void ATTPlayerController::OnAnimation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->OnAnimation ();
	}
}

void ATTPlayerController::EndAnimation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->EndAnimation ();
	}
}
void ATTPlayerController::WinAnimation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->WinAnimation ();
	}
}

void ATTPlayerController::LoseAnimation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->LoseAnimation ();
	}
}
void ATTPlayerController::DeadAnimation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->DeadAnimation ();
	}
}

void ATTPlayerController::DrawAnimation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->DrawAnimation ();
	}
}

void ATTPlayerController::ClientPlayStartAnim_Implementation ()
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->StartAnim ();
	}
}

void ATTPlayerController::ServerClientReady_Implementation ()
{
	if (HasAuthority ())
	{
		if (AInGameModeBase* GM = GetWorld ()->GetAuthGameMode<AInGameModeBase> ())
		{
			GM->Ready ();
		}
	}

}

void ATTPlayerController::ClientPlayingGame_Implementation ( int32 minutes , int32 seconds )const
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->CountDownTimer ( minutes , seconds );
	}
}

void ATTPlayerController::ClientAddportrait_Implementation ( const FString& PlayerName , UMaterialInstanceDynamic* portrait ) const
{
	if (IsValid ( TTInGameHUD ))
	{
		TTInGameHUD->Addportrait ( PlayerName, portrait );
	}
}

#pragma endregion