// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPlayerState.h"
#include "TTPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void ATTPlayerState::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );
	DOREPLIFETIME(ATTPlayerState, HeadMeshID );
	DOREPLIFETIME(ATTPlayerState, BodyMeshID );
	DOREPLIFETIME(ATTPlayerState, UserNickname);
	DOREPLIFETIME(ATTPlayerState, SelectedCharacterRowName);
}

void ATTPlayerState::SetHeadMeshData ( TSoftObjectPtr<USkeletalMesh> InData )
{
	HeadMeshID = InData;
	OnRep_HeadMeshData ();
}

void ATTPlayerState::SetBodyMeshData ( TSoftObjectPtr<USkeletalMesh> InData )
{
	BodyMeshID = InData;
	OnRep_BodyMeshData ();
}

void ATTPlayerState::OnRep_HeadMeshData ()
{
	APawn* Pawn = GetPawn ();
	if (!Pawn) return;

	if (ATTPlayerCharacter* Char = Cast<ATTPlayerCharacter> ( Pawn ))
	{
		Char->ApplyHeadMeshData ( HeadMeshID );
	}
}

void ATTPlayerState::OnRep_BodyMeshData ()
{
	APawn* Pawn = GetPawn ();
	if (!Pawn) return;

	if (ATTPlayerCharacter* Char = Cast<ATTPlayerCharacter> ( Pawn ))
	{
		Char->ApplyBodyMeshData ( BodyMeshID );
	}
}

void ATTPlayerState::OnRep_UserNickname()
{
	// Optional: Broadcast delegate if needed
}
