#include "TTPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "../InGameMode/InGameModeBase.h"
#include "../InGameUI/TTInGameHUD.h"
#include "../InGameUI/TTChatUI.h"
#include "../Character/TTPlayerCharacter.h"



ATTPlayerController::ATTPlayerController ()
	: InputMappingContext ( nullptr ) ,
	MoveAction ( nullptr ) ,
	JumpAction ( nullptr ) ,
	SprintAction ( nullptr )
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

