// Fill out your copyright notice in the Description page of Project Settings.


#include "TTInGameHUD.h"
#include "../InGameUI/TTChatUI.h"
#include "../InGameUI/TTChatMessage.h"
#include "../Controller/TTPlayerController.h"
#include "Components/ScrollBox.h"
#include "../InGameUI/TTSeletMeshs.h"

void ATTInGameHUD::PostInitializeComponents ()
{
	Super::PostInitializeComponents ();

	if(APlayerController* PC = GetOwningPlayerController())
	{
		OwningPlayer = Cast<ATTPlayerController> ( PC );
		if(ChatClass)
		{
			Chat = CreateWidget<UTTChatUI> ( PC , ChatClass );
		}
		if(ESCMenuClass)
		{
			ESCMenu = CreateWidget<UUserWidget> ( PC , ESCMenuClass );
		}
		if(SelectedSkeletalMeshClass)
		{
			SelectedSkeletalMesh = CreateWidget<UUserWidget> ( PC , SelectedSkeletalMeshClass );
		}
	}
}
#pragma region ChatUI

void ATTInGameHUD::AddChat () const
{

	if(IsValid(Chat))
	{
		Chat->AddToViewport ( );
	}
}

void ATTInGameHUD::AddChatMessage ( const FString& Message )
{

	if (OwningPlayer && ChatClass && Chat && ChatMessageClass)
	{
		
		if (UTTChatMessage* ChatMessageWidget = CreateWidget<UTTChatMessage> ( OwningPlayer , ChatMessageClass ))
		{
			ChatMessageWidget->SetChatMessage ( Message );
			Chat->ChatScrollBox->AddChild (ChatMessageWidget);
			Chat->ChatScrollBox->ScrollToEnd ();
			Chat->ChatScrollBox->bAnimateWheelScrolling = true;
		}
	}
}

#pragma endregion

#pragma region ESCMenu

void ATTInGameHUD::AddESCMenu () const
{
	if (IsValid ( ESCMenu ))
	{
		ESCMenu->AddToViewport ();
	}
}


#pragma endregion

#pragma region SelectedSkeletalMesh

void ATTInGameHUD::AddSelectedSkeletalMenu () const
{

	if (IsValid ( SelectedSkeletalMesh ))
	{
		SelectedSkeletalMesh->AddToViewport ();
	}
}

#pragma endregion

