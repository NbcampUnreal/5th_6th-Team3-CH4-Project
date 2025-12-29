// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameModeBase.h"
#include "Controller/TTPlayerController.h"
#include "Character/TTPlayerCharacter.h"
#include "Character/TTPlayerState.h"
#include "Outgame/TTGameInstance.h"
#include "InGameMode/TTGameStateBase.h"


// ----- Outgame 담당자가 추가함 ----- 
AInGameModeBase::AInGameModeBase () :
	InPlayerCount ( 0 ) ,
	PlayerCount ( 0 )
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
		EndRound ();
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
	Super::Logout ( ExitPlayer );
}

void AInGameModeBase::Ready ()
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
				if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( TTPC->GetPawn ()->GetPlayerState () ))
				{
					Sendportrait ( TTPS->GetUserNickname () , TTPS->PortraitMID );
				}

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
	int32 RedTeamCount = 0;
	int32 BlueTeamCount = 0;
	for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It) // 현제 접속중인 컨트롤러를 순회
	{
		APlayerController* PC = It->Get ();

		if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( PC->PlayerState ))
		{
			if (!IsValid ( TTPS )) // 유효성 검사
				continue;
			if (ATTPlayerCharacter* TTP = Cast<ATTPlayerCharacter> ( TTPS->GetPawn () ))
			{
				if (TTP->IsDead ()) // 컨트롤러의 캐릭터가 죽은 상태라면 체크하지 않음
				{
					//continue;
				}
			}
			if (TTPS->GetTeam () == Teams::Blue)
			{
				++BlueTeamCount;
			}
			else if (TTPS->GetTeam () == Teams::Red)
			{
				++RedTeamCount;
			}
		}
	}
	if (RedTeamCount == 0 || BlueTeamCount == 0)
	{
		Teams WinTeam = RedTeamCount == 0 ? Teams::Blue : Teams::Red;
		if (RedTeamCount == BlueTeamCount)
		{
			WinTeam = Teams::None;
		}
		for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It) // 현제 접속중인 컨트롤러를 순회
		{
			ATTPlayerController* TTPC = Cast<ATTPlayerController> ( It->Get () );

			if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( TTPC->PlayerState ))
			{
				if (WinTeam == Teams::None)
				{
					TTPC->DrawAnimation ();
					continue;
				}
				if (TTPS->GetTeam () == WinTeam)
				{
					// 승리 노티파이케이션 출력
					TTPC->WinAnimation ();
				}
				else
				{
					// 패배 노티파이케이션 출력
					TTPC->LoseAnimation ();
				}
			}
		}
		// 일정 시간 후 로비로 이동
		FTimerHandle EndHandle;
		GetWorldTimerManager ().SetTimer (
			EndHandle ,
			this ,
			&ThisClass::ServerTravelMap ,
			5.f ,
			false
		);
	}
	else if (bIsGameEnd)
	{
		Teams WinTeam = RedTeamCount > BlueTeamCount ? Teams::Red : Teams::Blue;
		WinTeam = RedTeamCount == BlueTeamCount ? Teams::None : WinTeam;
		for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It) // 현제 접속중인 컨트롤러를 순회
		{
			ATTPlayerController* TTPC = Cast<ATTPlayerController> ( It->Get () );

			if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( TTPC->PlayerState ))
			{
				if (WinTeam == Teams::None)
				{
					// 무승부 노티파이케이션 출력
					TTPC->DrawAnimation ();
					continue;
				}
				if (TTPS->GetTeam () == WinTeam)
				{
					// 승리 노티파이케이션 출력
					TTPC->WinAnimation ();
				}
				else
				{
					// 패배 노티파이케이션 출력
					TTPC->LoseAnimation ();
				}
			}
		}

		FTimerHandle EndHandle;
		GetWorldTimerManager ().SetTimer (
			EndHandle ,
			this ,
			&ThisClass::ServerTravelMap ,
			5.f ,
			false
		);
	}
}
#pragma endregion

void AInGameModeBase::Sendportrait ( const FString& PlayerName , UMaterialInstanceDynamic* portrait ) const
{
	for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It)
	{
		ATTPlayerController* PC = Cast<ATTPlayerController> ( *It );
		if (PC)
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "ServerAddportrait1" ) );
			PC->ClientAddportrait ( PlayerName , portrait );
		}
	}
}

void AInGameModeBase::ServerTravelMap ()
{
	bIsGameStart = false;
	bIsGameEnd = false;
	GetWorld ()->ServerTravel ( TEXT ( "/Game/Maps/LobbyLevel?listen" ) );
}
