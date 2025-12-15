#include "TTPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "../InGameMode/InGameModeBase.h"
#include "../InGameUI/TTInGameHUD.h"
#include "../InGameUI/TTChatUI.h"
#include "../Character/TTPlayerCharacter.h"
#include "../Outgame/TTGameInstance.h"
#include "../Character/TTPlayerState.h"
#include "../Save/TTSaveGame.h"
#include "Kismet/GameplayStatics.h"

ATTPlayerController::ATTPlayerController ()
	: InputMappingContext ( nullptr ) ,
	MoveAction ( nullptr ) ,
	JumpAction ( nullptr ) ,
	SprintAction ( nullptr ) ,
	AttackAction ( nullptr ) ,
	BlockingAction ( nullptr ) 
{
	bReplicates = true;
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

#pragma endregion

#pragma region SelectMash
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

void ATTPlayerController::ServerRequestChangeHeadMesh_Implementation (const TSoftObjectPtr<USkeletalMesh>& HeadID )
{
	ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ();
	if (!PS) return;

	FCharacterMeshData NewData;
	NewData.HeadMeshID = HeadID;
	NewData.BodyMeshID = PS->MeshData.BodyMeshID;

	PS->SetMeshData ( NewData );
}

void ATTPlayerController::ServerRequestChangeBodyMesh_Implementation (const TSoftObjectPtr<USkeletalMesh>& BodyID )
{
	ATTPlayerState* PS = GetPlayerState<ATTPlayerState> ();
	if(!IsValid( PS))
		return;

	FCharacterMeshData NewData;
	NewData.HeadMeshID = PS->MeshData.HeadMeshID;
	NewData.BodyMeshID = BodyID;

	PS->SetMeshData ( NewData );
}


#pragma endregion

