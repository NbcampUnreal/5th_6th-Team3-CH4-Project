 // Fill out your copyright notice in the Description page of Project Settings.


#include "TTPlayerState.h"
#include "TTPlayerCharacter.h"
#include "TTLobbyCharacter.h"
#include "../Save/TTSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"


void ATTPlayerState::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );
	DOREPLIFETIME ( ATTPlayerState , PersistedHeadMesh );
	DOREPLIFETIME ( ATTPlayerState , PersistedBodyMesh );
	//DOREPLIFETIME(ATTPlayerState, PortraitMID );
	DOREPLIFETIME(ATTPlayerState, UserNickname);
	DOREPLIFETIME(ATTPlayerState, SelectedCharacterRowName);
	DOREPLIFETIME(ATTPlayerState, Team );
    DOREPLIFETIME(ATTPlayerState, bIsReady);
    DOREPLIFETIME(ATTPlayerState, bIsHost);
}

void ATTPlayerState::CopyProperties ( APlayerState* NewPlayerState )
{
	Super::CopyProperties ( NewPlayerState );

	if (ATTPlayerState* PS = Cast<ATTPlayerState> ( NewPlayerState ))
	{
		PS->Team = Team;
		UE_LOG ( LogTemp , Warning , TEXT ( "[CopyProperties] COPIED Team=%d" ) , (uint8)PS->Team );
	}
	else
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "[CopyProperties] Cast FAILED" ) );
	}
}

void ATTPlayerState::OverrideWith ( APlayerState* OldPlayerState )
{
	Super::OverrideWith ( OldPlayerState );
	if (ATTPlayerState* PS = Cast<ATTPlayerState> ( OldPlayerState ))
	{
		Team = PS->Team;
	}
}

void ATTPlayerState::SetTeam_Implementation ( Teams NewTeam )
{
	Team = NewTeam;
	ForceNetUpdate ();
}

void ATTPlayerState::SetPortraitRenderTarget ( UTextureRenderTarget2D* InRT )
{
	if (!IsValid( InRT ) || !IsValid ( PortraitBaseMaterial ))
		return;

	PortraitMID = UMaterialInstanceDynamic::Create ( PortraitBaseMaterial , this );
	static const FName PortraitParamName = TEXT ( "PortraitTexture" );
	PortraitMID->SetTextureParameterValue ( PortraitParamName , InRT );
}

void ATTPlayerState::OnRep_UserNickname()
{
	// Optional: Broadcast delegate if needed
}

void ATTPlayerState::OnRep_Team ()
{

}

void ATTPlayerState::OnRep_IsReady()
{
	if (APawn* MyPawn = GetPawn())
	{
		if (ATTLobbyCharacter* LobbyChar = Cast<ATTLobbyCharacter>(MyPawn))
        {
            LobbyChar->PlayReadyMontage(bIsReady);
        }
	}
}

