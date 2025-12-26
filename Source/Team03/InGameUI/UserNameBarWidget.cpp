// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/UserNameBarWidget.h"
#include "Character/TTPlayerState.h"
#include "Components/TextBlock.h"

void UUserNameBarWidget::NativeConstruct ()
{
	if (ATTPlayerState* TTPS = Cast<ATTPlayerState> ( GetOwningPlayerState () ))
	{
		const FName UserName = *TTPS->GetUserNickname ();
		SetName ( UserName );
	}
}

void UUserNameBarWidget::SetName (const FName& UserName )
{
	PlayerName->SetText ( FText::FromName ( UserName ) );
}
