// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/TTNotificationWidget.h"
#include "Components/TextBlock.h"

void UTTNotificationWidget::NativeConstruct ()
{
	NotificationText->SetText ( FText::FromString ( TEXT ( "Loding..." ) ) );
}

void UTTNotificationWidget::PlayStartAnim ()
{
	PlayAnimation ( StartAnim );
	NotificationText->SetText ( FText::FromString ( TEXT ( "%d : %02d" , 3 , 00 ) ) );

}

void UTTNotificationWidget::CountDownTimer (int32 minutes, int32 seconds )const
{
	NotificationText->SetText ( FText::FromString ( TEXT ( "%d : %d", minutes, seconds) ) );
}
