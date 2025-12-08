// Fill out your copyright notice in the Description page of Project Settings.


#include "TTChatUI.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"

void UTTChatUI::NativeConstruct ()
{
	Super::NativeConstruct ();
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->OnTextCommitted.AddDynamic ( this , &UTTChatUI::OnTextCommitted );
	}
	ChatInputBox->SetIsEnabled ( false );
}

void UTTChatUI::NativeDestruct ()
{
	Super::NativeDestruct ();
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->OnTextCommitted.RemoveDynamic ( this , &UTTChatUI::OnTextCommitted );
	}
}

void UTTChatUI::SetChatMessage ( const FString& Message )
{
	if (IsValid(ChatInputBox))
	{
		ChatInputBox->SetText ( FText::FromString ( Message ) );
	}
}
void UTTChatUI::ActivateChat ()
{
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->SetIsEnabled ( true );
		ChatInputBox->SetKeyboardFocus ();
	}
}
void UTTChatUI::ServerRPCPrintString_Implementation ( const FString& InMessage )
{
	MulticastRPCPrintString ( InMessage );
}
void UTTChatUI::MulticastRPCPrintString_Implementation ( const FString& InMessage )
{
}
void UTTChatUI::OnTextCommitted ( const FText& Text , ETextCommit::Type CommitMethod )
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (IsValid( ChatInputBox))
		{
			FText InputText = ChatInputBox->GetText ();
			FString Message = InputText.ToString ().TrimStartAndEnd ();

			if(!Message.IsEmpty())
			{
				ServerRPCPrintString( Message );
			}
		}
	}
}
