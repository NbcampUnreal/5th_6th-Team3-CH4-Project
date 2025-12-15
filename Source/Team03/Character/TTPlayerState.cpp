// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPlayerState.h"
#include "Net/UnrealNetwork.h"

void ATTPlayerState::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );
	DOREPLIFETIME ( ATTPlayerState , PersistedHeadMesh );
	DOREPLIFETIME ( ATTPlayerState , PersistedBodyMesh );
	DOREPLIFETIME(ATTPlayerState, UserNickname);
	DOREPLIFETIME(ATTPlayerState, SelectedCharacterRowName);
}

void ATTPlayerState::OnRep_UserNickname()
{
	// Optional: Broadcast delegate if needed
}
