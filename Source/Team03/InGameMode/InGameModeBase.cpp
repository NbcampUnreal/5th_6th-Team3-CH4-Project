// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameModeBase.h"
#include "../Controller/TTPlayerController.h"

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
