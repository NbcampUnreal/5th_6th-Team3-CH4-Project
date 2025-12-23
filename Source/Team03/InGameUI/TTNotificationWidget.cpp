// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/TTNotificationWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UTTNotificationWidget::NativeConstruct ()
{
	NotificationText->SetText ( FText::FromString ( TEXT ( "Loding..." ) ) );
}

void UTTNotificationWidget::PlayStartAnim ()
{
	PlayAnimation ( StartAnim );
	NotificationText->SetText (
		FText::Format (
			FText::FromString ( TEXT ( "{0} : {2:00}" ) ) ,
			3 ,
			0
		)
	);
}

void UTTNotificationWidget::CountDownTimer (int32 minutes, int32 seconds )const
{
	NotificationText->SetText (
		FText::Format (
			FText::FromString ( TEXT ( "{0} : {2:00}" ) ) ,
			minutes ,
			seconds
		)
	);
}
