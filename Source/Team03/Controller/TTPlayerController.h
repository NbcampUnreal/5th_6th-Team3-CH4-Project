// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTPlayerController : public APlayerController
{
	GENERATED_BODY()
#pragma region ChatUI
public:
	UFUNCTION ()
	void ActivateChatBox ();

	UFUNCTION ( Server , Reliable )
	void ServerSendChatMessage ( const FString& Message );

	UFUNCTION ( Client , Reliable )
	void ClientAddChatMessage ( const FString& Message );

private:
	UPROPERTY ()
	TObjectPtr< class ATTInGameHUD >  TTInGameHUD;
#pragma endregion
};
