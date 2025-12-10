// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class TEAM03_API ATTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATTPlayerController ();


	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputMappingContext* InputMappingContext;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* MoveAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* JumpAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* SprintAction;

	virtual void BeginPlay () override;
	
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

#pragma region ESCMenu
public:
	UFUNCTION ()
	void ActivateESCMenu ();
#pragma endregion

#pragma region SelectedSkeletalMesh
public:
	UFUNCTION ()
	void ActivateSelectedSkeletalMesh ();

	UFUNCTION()
	void ChangeHeadMesh ( USkeletalMesh* NewMesh );

	UFUNCTION()
	void ChangeMesh ( USkeletalMesh* NewMesh );

#pragma endregion

};
