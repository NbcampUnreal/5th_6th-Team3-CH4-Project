// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/TTPlayerPortraitWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UTTPlayerPortraitWidget::SetNameText ( const FString& Name )
{
	PlayerName->SetText ( FText::FromString (Name));
}

