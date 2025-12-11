#include "TTPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "../InGameMode/InGameModeBase.h"
#include "../InGameUI/TTInGameHUD.h"
#include "../InGameUI/TTChatUI.h"
#include "../Character/TTPlayerCharacter.h"
#include "../Outgame/TTGameInstance.h"
#include "../Character/TTPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../Save/TTSaveGame.h"



ATTPlayerController::ATTPlayerController ()
	: InputMappingContext ( nullptr ) ,
	MoveAction ( nullptr ) ,
	JumpAction ( nullptr ) ,
	SprintAction ( nullptr )
{
	bReplicates = true;
}

void ATTPlayerController::OnPossess ( APawn* InPawn )
{
	//LoadPlayerSaveData ( TEXT ( "MySaveSlot_01" ) , 0 );
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
	}

// ---------- Outgame 담당자가 추가한 코드 ----------
	if (IsLocalController())
	{
		// Force Game Input Mode
		FInputModeGameOnly GameInputMode;
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;

		if (UTTGameInstance* GI = Cast<UTTGameInstance>(GetGameInstance()))
		{
			UE_LOG(LogTemp, Log, TEXT("[TTPlayerController] Sending GameInstance Data to Server. Nickname: %s, RowName: %s"), *GI->UserNickname, *GI->SelectedCharacterRowName.ToString());
			ServerRPC_InitPlayerInfo(GI->UserNickname, GI->SelectedCharacterRowName);
		}
	}
}

void ATTPlayerController::ServerRPC_InitPlayerInfo_Implementation(const FString& Nickname, const FName& CharacterRowName)
{
	UE_LOG(LogTemp, Log, TEXT("[TTPlayerController] Server Received Data. Nickname: %s, RowName: %s"), *Nickname, *CharacterRowName.ToString());

	if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
	{
		PS->UserNickname = Nickname;
		PS->SelectedCharacterRowName = CharacterRowName;

		// Force update if needed, but replication will handle it
		PS->ForceNetUpdate();
	}
}

bool ATTPlayerController::ServerRPC_InitPlayerInfo_Validate(const FString& Nickname, const FName& CharacterRowName)
{
	return true;
}
// ---------- Outgame 담당자가 추가한 코드 ----------

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
		GM->SendChatMessage ( Message );
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
	UE_LOG ( LogTemp , Warning , TEXT ( "SavePlayerSaveData()" ) );

	ATTPlayerCharacter* OwnerPlayer = Cast<ATTPlayerCharacter> ( GetPawn () );

	if (IsValid ( OwnerPlayer->Head ) && IsValid ( OwnerPlayer->GetMesh () ) && IsValid( OwnerPlayer ))
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "1" ) );
		USaveGame* SaveGameInstance = UGameplayStatics::CreateSaveGameObject ( UTTSaveGame::StaticClass () );
		if (!SaveGameInstance)
			return;
		UE_LOG ( LogTemp , Warning , TEXT ( "2" ) );

		UTTSaveGame* TTSaveGameInstance = Cast<UTTSaveGame> ( SaveGameInstance );
		if (!TTSaveGameInstance)
			return;
		UE_LOG ( LogTemp , Warning , TEXT ( "3" ) );
		// 세이브 데이터에 세이브
		if (USkeletalMesh* CurrentHeadMesh = OwnerPlayer->Head->GetSkeletalMeshAsset ())
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "4" ) );
			TTSaveGameInstance->CurrentHeadMeshPath = FSoftObjectPath ( CurrentHeadMesh );
		}
		if (USkeletalMesh* CurrentBodyMesh = OwnerPlayer->GetMesh ()->GetSkeletalMeshAsset ())
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "5" ) );
			TTSaveGameInstance->CurrentBodyMeshPath = FSoftObjectPath ( CurrentBodyMesh );
		}
		UE_LOG ( LogTemp , Warning , TEXT ( "6" ) );

		UGameplayStatics::SaveGameToSlot ( TTSaveGameInstance , SlotName , UserIndex );
	}
}

void ATTPlayerController::LoadPlayerSaveData ( const FString& SlotName , int32 UserIndex )
{
	ATTPlayerCharacter* OwnerPlayer = Cast<ATTPlayerCharacter> ( GetPawn () );
	UE_LOG ( LogTemp , Warning , TEXT ( "1" ) );

	if (IsValid ( OwnerPlayer->Head ) && IsValid ( OwnerPlayer->GetMesh () ) && IsValid( OwnerPlayer ))
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "2" ) );

		// 세이브 데이터를 불러오는 과정
		USaveGame* LoadGameInstance = UGameplayStatics::CreateSaveGameObject ( UTTSaveGame::StaticClass () );
		if (!LoadGameInstance)
			return;
		UE_LOG ( LogTemp , Warning , TEXT ( "3" ) );

		UTTSaveGame* TTLoadGameInstance = Cast<UTTSaveGame> ( LoadGameInstance );
		if (!TTLoadGameInstance)
			return;

		UE_LOG ( LogTemp , Warning , TEXT ( "4" ) );

		USkeletalMesh* LoadedHeadMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentHeadMeshPath.ResolveObject () );
		if (!TTLoadGameInstance->CurrentHeadMeshPath.IsNull () && !TTLoadGameInstance->CurrentBodyMeshPath.IsNull ())
			return;
		UE_LOG ( LogTemp , Warning , TEXT ( "5" ) );

		// 로드된 세이브 데이터를 불러오는 과정 
		if (IsValid ( TTLoadGameInstance->CurrentHeadMeshPath.ResolveObject () ) && IsValid ( TTLoadGameInstance->CurrentBodyMeshPath.ResolveObject () ))
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "6" ) );
			USkeletalMesh* LoadHeadMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentHeadMeshPath.ResolveObject () );
			USkeletalMesh* LoadBodyMesh = Cast<USkeletalMesh> ( TTLoadGameInstance->CurrentBodyMeshPath.ResolveObject () );
			// 세이브 데이터 적용
			if (IsValid ( LoadHeadMesh ) && IsValid ( LoadBodyMesh ))
			{
				UE_LOG ( LogTemp , Warning , TEXT ( "7" ) );
				OwnerPlayer->ServerChangeHeadMesh ( LoadHeadMesh );
				OwnerPlayer->ServerChangeBodyMesh ( LoadBodyMesh );
			}

		}

	}
}
#pragma endregion
