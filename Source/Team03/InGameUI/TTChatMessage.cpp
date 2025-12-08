// Fill out your copyright notice in the Description page of Project Settings.


#include "TTChatMessage.h"
#include "Components/TextBlock.h"

void UTTChatMessage::SetChatMessage ( const FString& Message )
{
	if(IsValid( ChatTextBox ))
	{
		ChatTextBox->SetText ( FText::FromString ( Message ) );
	}
}
