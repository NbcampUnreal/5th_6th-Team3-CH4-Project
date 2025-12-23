// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameModeBase.h"
#include "Controller/TTPlayerController.h"
#include "Character/TTPlayerCharacter.h"
#include "Character/TTPlayerState.h"
#include "Outgame/TTGameInstance.h"
#include "InGameMode/TTGameStateBase.h"


// ----- Outgame 담당자가 추가함 ----- 
AInGameModeBase::AInGameModeBase() :
	InPlayerCount(0),
	PlayerCount(0)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseSeamlessTravel = true;
}

void AInGameModeBase::BeginPlay ()
{
	UTTGameInstance* GI = GetGameInstance<UTTGameInstance> ();
	if (IsValid ( GI ))
	{
		PlayerCount = GI->count;
	}
}

void AInGameModeBase::Tick ( float Delatasecond )
{
	Super::Tick ( Delatasecond );
	if (!bIsGameStart)
	{
		StartRound ();
	}
	else
	{

		//EndRound ();
	}
}

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
#pragma region Ready
void AInGameModeBase::PostLogin ( APlayerController* NewPlayer )
{
	Super::PostLogin ( NewPlayer );
	UE_LOG ( LogTemp , Warning , TEXT ( "PostLogin" ) );
}

void AInGameModeBase::Logout ( AController* ExitPlayer )
{
	--InPlayerCount;
	--PlayerCount;
	EndRound ();
	Super::Logout ( ExitPlayer );
}

void AInGameModeBase::Ready()
{
	++InPlayerCount;
}
#pragma endregion

#pragma region Start
void AInGameModeBase::StartRound ()
{
	if (InPlayerCount == PlayerCount)
	{
		FTimerHandle StartHandle;
			GetWorldTimerManager ().SetTimer (
				StartHandle ,
				this ,
				&ThisClass::PlayingGame ,
				4.f ,
				false
			);
			
		for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It)
		{
			if (ATTPlayerController* TTPC = Cast<ATTPlayerController> ( It->Get () ))
			{
				TTPC->ClientPlayStartAnim ();
			}
		}
		bIsGameStart = true;
	}
}

void AInGameModeBase::PlayingGame ()
{
	GetWorldTimerManager ().SetTimer (
		TimeCountHandle ,
		this ,
		&ThisClass::CountDownTimer ,
		1.f ,
		true
	);

}

void AInGameModeBase::CountDownTimer ()
{
	if (--seconds < 0)
	{
		if (--minutes < 0)
		{
			GetWorld ()->GetTimerManager ().ClearTimer ( TimeCountHandle );
			bIsGameEnd = true;
			return;
		}
		seconds = 59;
	}
	for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It)
	{
		if (ATTPlayerController* TTPC = Cast<ATTPlayerController> ( It->Get () ))
		{
			TTPC->ClientPlayingGame ( minutes , seconds );
		}
	}
}
#pragma endregion

#pragma region end
void AInGameModeBase::EndRound ()
{
	Teams T = Teams::None;
	for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It)
	{
		APlayerController* PC = It->Get ();

		if(ATTPlayerState* TTPS = Cast<ATTPlayerState> ( PC->PlayerState ))
		{
			if (!IsValid ( TTPS ))
				continue;
			if (ATTPlayerCharacter* TTP = Cast<ATTPlayerCharacter> ( TTPS->GetPawn () ))
			{
				if (TTP->IsDead ())
					continue;
			}
			if (T == Teams::None)
				T = TTPS->GetTeam ();
			else
			{
				if (T != TTPS->GetTeam ())
					return;
			}
		}
	}
		
	if (T == Teams::Blue)
	{
		// 블루팀 승리
		UE_LOG ( LogTemp , Warning , TEXT ( "Win BlueTeam" ) );
	}
	if (T == Teams::Red)
	{
		// 레드팀 승리
		UE_LOG ( LogTemp , Warning , TEXT ( "Win RedTeam" ) );
	}
}
#pragma endregion
