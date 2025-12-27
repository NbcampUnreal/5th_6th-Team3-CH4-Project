// Fill out your copyright notice in the Description page of Project Settings.


#include "TTInGameHUD.h"
#include "InGameUI/TTChatUI.h"
#include "InGameUI/TTChatMessage.h"
#include "Controller/TTPlayerController.h"
#include "Components/ScrollBox.h"
#include "InGameUI/TTSeletMeshs.h"
#include "InGameUI/TTNotificationWidget.h"
#include "Outgame/TTGameInstance.h"
#include "InGameUI/TTPlayerPortraitWidget.h"
void ATTInGameHUD::PostInitializeComponents ()
{
	Super::PostInitializeComponents ();

		
	if (UTTGameInstance* GI = Cast<UTTGameInstance> ( GetGameInstance () ))
	{
		GI->PlayBGM ( DesertBGM );
	}
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
		if(NotificationClass)
		{
			Notification = CreateWidget<UTTNotificationWidget> ( PC , NotificationClass );
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
 			Chat->ChatScrollBox->SetAnimateWheelScrolling(true);
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

#pragma region NotificationWidget
void ATTInGameHUD::AddNotification () const
{
	if (IsValid ( Notification ))
	{
		Notification->AddToViewport ();
	}
}

void ATTInGameHUD::StartAnim () const
{
	if (IsValid ( Notification ))
	{
		Notification->PlayStartAnim ();
	}

}

void ATTInGameHUD::CountDownTimer ( int32 minutes , int32 seconds )const
{
	if (IsValid ( Notification ))
	{
		Notification->CountDownTimer ( minutes , seconds );
	}
}
void ATTInGameHUD::OnAnimation ()const
{
	if (IsValid ( Notification ))
	{
		Notification->OnAnimation ();
	}
}
void ATTInGameHUD::EndAnimation ()const
{
	if (IsValid ( Notification ))
	{
		Notification->EndAnimation ( );
	}
}

void ATTInGameHUD::Addportrait (const FString& PlayerName, UMaterialInstanceDynamic* portrait ) const
{
	if (IsValid ( Notification ))
	{
		if (!PortraitWidgetClass)
		{
			return;
		}
		if (APlayerController* PC = GetOwningPlayerController ())
		{
			UTTPlayerPortraitWidget* Widget = CreateWidget<UTTPlayerPortraitWidget> ( PC , PortraitWidgetClass );
			Widget->SetNameText ( PlayerName );
			Notification->PlayerportraitBox->AddChild ( Widget );
		}
	}
}


#pragma endregion

