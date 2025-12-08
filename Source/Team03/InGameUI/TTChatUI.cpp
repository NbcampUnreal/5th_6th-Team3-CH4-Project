// Fill out your copyright notice in the Description page of Project Settings.


#include "TTChatUI.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"

// 생성자
void UTTChatUI::NativeConstruct ()
{
	Super::NativeConstruct ();
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->OnTextCommitted.AddDynamic ( this , &UTTChatUI::OnTextCommitted );
	}
	ChatInputBox->SetIsEnabled ( false );
}

// 소멸자
void UTTChatUI::NativeDestruct ()
{
	Super::NativeDestruct ();
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->OnTextCommitted.RemoveDynamic ( this , &UTTChatUI::OnTextCommitted );
	}
}

// 엔터시 진입할 함수이며 메세지입력
void UTTChatUI::ActivateChat ()
{
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->SetIsEnabled ( true );
		ChatInputBox->SetKeyboardFocus ();
	}
}

// 엔터로 커밋시 호출되는 함수
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

				ChatInputBox->SetText ( FText::GetEmpty () );
				ChatInputBox->SetIsEnabled ( false );
			}
		}
	}
}
