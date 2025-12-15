// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPlayerState.h"
#include "TTPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void ATTPlayerState::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );
	DOREPLIFETIME(ATTPlayerState, MeshData );
	DOREPLIFETIME(ATTPlayerState, UserNickname);
	DOREPLIFETIME(ATTPlayerState, SelectedCharacterRowName);
}

void ATTPlayerState::SetMeshData ( FCharacterMeshData InData )
{
	MeshData = InData;
	OnRep_MeshData ();
}

void ATTPlayerState::OnRep_MeshData ()
{
	APawn* Pawn = GetPawn ();
	if (!Pawn) return;

	if (ATTPlayerCharacter* Char = Cast<ATTPlayerCharacter> ( Pawn ))
	{
		Char->ApplyMeshData ( MeshData );
	}
}

void ATTPlayerState::OnRep_UserNickname()
{
	// Optional: Broadcast delegate if needed
}
