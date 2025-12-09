// Fill out your copyright notice in the Description page of Project Settings.


#include "TTChatUI.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "../Controller/TTPlayerController.h"

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

// 엔터시 진입할 함수이며 메세지입력
void UTTChatUI::ActivateChat ()
{
	if (IsValid ( ChatInputBox ) )
	{
		ChatInputBox->SetIsEnabled ( true );
		ChatInputBox->SetFocus ();
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
				if(ATTPlayerController* PC = Cast<ATTPlayerController>( GetOwningPlayer () ))
				{
					// ToDo : 플레이어 캐릭터 이름 채팅에 추가
					//TObjectPtr <APlayerState> PS = PC->GetPlayerState<APlayerState> ();
					//FString Message = FString::Printf ( TEXT ( "%s : %s" ) , TObjectPtr < APlayerState>->GetPlayerName() , *Message );
					PC->ServerSendChatMessage ( Message );

					FInputModeGameOnly InputMode;
					PC->SetInputMode ( InputMode );

					ChatInputBox->SetText ( FText::GetEmpty () );
					ChatInputBox->SetIsEnabled ( false );
				}
			}
		}
	}
}
