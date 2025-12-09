// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTitlePlayerController.h"

ATTTitlePlayerController::ATTTitlePlayerController()
{
	// SRS 4.2: Mouse Cursor Show
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ATTTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputMode(FInputModeUIOnly());
}
