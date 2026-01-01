// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameModeBase.h"
#include "Controller/TTPlayerController.h"
#include "Character/TTPlayerCharacter.h"
#include "Character/TTPlayerState.h"
#include "Outgame/TTGameInstance.h"
#include "InGameMode/TTGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

//스폰
#include "GameFramework/PlayerStart.h"

AActor* AInGameModeBase::ChoosePlayerStart_Implementation ( AController* Player )
{
	ATTPlayerState* TTPS = Player->GetPlayerState<ATTPlayerState> ();
	FName TargetTag = NAME_None;
	int32* CurrentTeamIndex = nullptr;

	if (TTPS)
	{
		if (TTPS->GetTeam () == Teams::Blue)
		{
			TargetTag = FName ( "BlueTeam" );
			CurrentTeamIndex = &BlueSpawnIndex;
		}

		else if (TTPS->GetTeam () == Teams::Red)
		{
			TargetTag = FName ( "RedTeam" );
			CurrentTeamIndex = &RedSpawnIndex;
		}
	}

	TArray<AActor*> FoundStarts;
	UGameplayStatics::GetAllActorsOfClass ( GetWorld () , APlayerStart::StaticClass () , FoundStarts );

	TArray<APlayerStart*> ValidStarts;
	for (AActor* Start : FoundStarts)
	{
		APlayerStart* PStart = Cast<APlayerStart> ( Start );
		if (PStart && PStart->PlayerStartTag == TargetTag)
		{
			ValidStarts.Add ( PStart );
		}
	}

	if (ValidStarts.Num () > 0 && CurrentTeamIndex != nullptr)
	{
		int32 SelectedIndex = (*CurrentTeamIndex) % ValidStarts.Num ();

		(*CurrentTeamIndex)++;

		return ValidStarts[SelectedIndex];
	}

	return Super::ChoosePlayerStart_Implementation ( Player );
}

// ----- Outgame 담당자가 추가함 ----- 
AInGameModeBase::AInGameModeBase () :
	InPlayerCount ( 0 ) ,
	PlayerCount ( 0 )
{
	PrimaryActorTick.bCanEverTick = true;
	bUseSeamlessTravel = true;
	BlueTeamCount = 0;
	RedTeamCount = 0;
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
					if (TTPS->GetTeam () == Teams::Blue)
					{
						++BlueTeamCount;
					}
					if (TTPS->GetTeam () == Teams::Red)
					{
						++RedTeamCount;
					}
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

	seconds -= 1;
	UE_LOG ( LogTemp , Warning , TEXT ( "seconds : %d" ) , seconds );
	if (seconds < 0)
	{
		minutes -= 1;
		UE_LOG ( LogTemp , Warning , TEXT ( "minutes : %d" ) , minutes );

		if (minutes < 0)
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "minutes : %d, seconds : %d" ) , minutes , seconds );
			GetWorld ()->GetTimerManager ().ClearTimer ( TimeCountHandle );
			bTimeUp = 1;
			EndRound ();
			return;
		}
		seconds = 59;
	}
	if (RedTeamCount == 0 || BlueTeamCount == 0)
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "else if : BlueTeamCount : %d, RedTeamCount : %d" ) , BlueTeamCount , RedTeamCount );
		minutes = 0;
		seconds = 0;
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
	//int32 RedTeamCount = 0;
	//int32 BlueTeamCount = 0;
	//for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It) // 현제 접속중인 컨트롤러를 순회
	//{
	//	APlayerController* PC = It->Get ();

	//	if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( PC->PlayerState ))
	//	{
	//		if (!IsValid ( TTPS )) // 유효성 검사
	//		{
	//			continue;
	//		}
	//		if (ATTPlayerCharacter* TTP = Cast<ATTPlayerCharacter> ( TTPS->GetPawn () ))
	//		{
	//			if (TTP->IsDead ()) // 컨트롤러의 캐릭터가 죽은 상태라면 체크하지 않음
	//			{
	//				UE_LOG ( LogTemp , Warning , TEXT ( "%s dead" ) , *TTPS->GetUserNickname () );
	//				continue;
	//			}
	//			else
	//			{
	//				UE_LOG ( LogTemp , Warning , TEXT ( "%s Not dead" ) , *TTPS->GetUserNickname () );
	//			}
	//			if (TTPS->GetTeam () == Teams::Blue)
	//			{
	//				++BlueTeamCount;
	//			}
	//			else if (TTPS->GetTeam () == Teams::Red)
	//			{
	//				++RedTeamCount;
	//			}
	//		}
	//	}
	//}
	//UE_LOG ( LogTemp , Warning , TEXT ( "BlueTeamCount : " ) , *TTPS->GetUserNickname () );

	if (bTimeUp == 1)
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "BlueTeamCount : %d, RedTeamCount : %d" ) , BlueTeamCount , RedTeamCount );
		Teams WinTeam = RedTeamCount > BlueTeamCount ? Teams::Red : Teams::Blue;
		WinTeam = RedTeamCount == BlueTeamCount ? Teams::None : WinTeam;
		for (FConstPlayerControllerIterator It = GetWorld ()->GetPlayerControllerIterator (); It; ++It) // 현제 접속중인 컨트롤러를 순회
		{
			ATTPlayerController* TTPC = Cast<ATTPlayerController> ( It->Get () );

			if (!IsValid ( TTPC ))
			{
				continue;
			}
			TTPC->ClientShowResult ( WinTeam );
		}
		if (HasAuthority ())
		{
			GetWorldTimerManager ().SetTimer (
				EndHandle ,
				this ,
				&ThisClass::ServerTravelMap ,
				5.f ,
				false
			);
		}

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
			PC->ClientAddportrait ( PlayerName , portrait );
		}
	}
}

void AInGameModeBase::ServerTravelMap ()
{
	if (!HasAuthority ())
	{
		return; 
	}

	GetWorld ()->ServerTravel ( TEXT ( "/Game/Maps/LobbyLevel?listen" ) , /*bAbsolute=*/true );
	
	// 호스트에서만 실행이 된다
	UKismetSystemLibrary::PrintString ( GetWorld () , TEXT ( "ServerTravelMap" ) , true , true , FLinearColor::Green , 2.f );
	//GEngine->AddOnScreenDebugMessage ( -1 , 2.f , FColor::Blue , TEXT ( "ServerTravelMap" ) );
}

