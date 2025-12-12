// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameModeBase.h"
#include "Controller/TTPlayerController.h"
#include "Character/TTPlayerCharacter.h"
#include "Character/TTPlayerState.h"

void AInGameModeBase::SendChatMessage ( const FString& Message )
{
	for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It)
	{
		ATTPlayerController* PC = Cast<ATTPlayerController> ( *It );
		if (PC)
		{
			PC->ClientAddChatMessage ( Message );
		}
	}
}

void AInGameModeBase::PostLogin ( APlayerController* NewPlayer )
{
	Super::PostLogin ( NewPlayer );
	UE_LOG ( LogTemp , Warning , TEXT ( "PostLogin" ) );

	if (ATTPlayerCharacter* TTPC = Cast<ATTPlayerCharacter> ( NewPlayer->GetPawn () ))
	{
		if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( TTPC->GetPlayerState() ))
		{
			TTPC->InitializeMesh( TTPS );
		}
	}
}
void AInGameModeBase::HandleStartingNewPlayer ( APlayerController* NewPlayer )
{
	// 1. **주의:** 부모 함수를 먼저 호출해야 GameMode의 기본 스폰 로직이 실행됩니다.
	Super::HandleStartingNewPlayer ( NewPlayer );

	// 2. 이 지점은 1차, 2차 InGameMode 모두에서 PlayerController가 레벨에 들어올 때 호출됩니다.

	// 3. 여기서 PlayerController에게 필요한 명령을 내립니다.
	ATTPlayerController* PC = Cast<ATTPlayerController> ( NewPlayer );
	if (PC)
	{
		// PC에 저장된 SaveGame 정보를 Pawn에 동기화하도록 지시하거나,
		// UI를 설정하는 등의 작업을 여기서 처리합니다.
		PC->Client_InitializeGameSession ();
	}
}