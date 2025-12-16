// (c) 2024. Team03. All rights reserved.

#include "TTTitlePlayerController.h"

ATTTitlePlayerController::ATTTitlePlayerController()
{
	// 마우스 커서 표시
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ATTTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputMode(FInputModeUIOnly());
}
