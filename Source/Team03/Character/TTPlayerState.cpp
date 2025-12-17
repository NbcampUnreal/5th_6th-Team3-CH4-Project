// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPlayerState.h"
#include "TTPlayerCharacter.h"
#include "../Save/TTSaveGame.h"
#include "Net/UnrealNetwork.h"



void ATTPlayerState::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );
	DOREPLIFETIME ( ATTPlayerState , PersistedHeadMesh );
	DOREPLIFETIME ( ATTPlayerState , PersistedBodyMesh );
	DOREPLIFETIME(ATTPlayerState, UserNickname);
	DOREPLIFETIME(ATTPlayerState, SelectedCharacterRowName);
	DOREPLIFETIME(ATTPlayerState, Team );
}

void ATTPlayerState::SetTeam ( Teams NewTeam )
{
	if (HasAuthority ())
	{
		Team = NewTeam;
		OnRep_Team (); // 서버에서도 즉시 반영
	}
}

void ATTPlayerState::OnRep_UserNickname()
{
	// Optional: Broadcast delegate if needed
}

void ATTPlayerState::OnRep_Team ()
{

}

