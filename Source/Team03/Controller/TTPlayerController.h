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

	virtual void SetPawn ( APawn* InPawn ) override;
	virtual void OnPossess ( APawn* InPawn ) override;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputMappingContext* InputMappingContext;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* MoveAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* JumpAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* SprintAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* AttackAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* BlockingAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* Dance1Action;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* Dance2Action;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* Dance3Action;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* Dance4Action;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* Dance5Action;

	virtual void BeginPlay () override;

protected:

	virtual void OnRep_Pawn () override;
	void PlayerSetUp ();
	
#pragma region ChatUI
public:
	UFUNCTION ()
	void ActivateChatBox ();

	UFUNCTION ( Server , Reliable )
	void ServerSendChatMessage ( const FString& Message );

	UFUNCTION ( Client , Reliable )
	void ClientAddChatMessage ( const FString& Message );

// Outgame 담당자가 추가한 코드
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_InitPlayerInfo(const FString& Nickname, const FName& CharacterRowName, int32 HeadIndex, int32 BodyIndex);

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

#pragma region SaveData
public:
	void SavePlayerSaveData ( const FString& SlotName , int32 UserIndex );
	void LoadPlayerSaveData ( const FString& SlotName , int32 UserIndex );

#pragma endregion

#pragma region Notification
public:
	UFUNCTION ()
	void OnAnimation ();
	UFUNCTION ()
	void EndAnimation ();

	UFUNCTION ()
	void WinAnimation ();
	UFUNCTION ()
	void LoseAnimation ();
	UFUNCTION ()
	void DeadAnimation ();

	UFUNCTION ( Client , Reliable )
	void ClientPlayStartAnim ();

	UFUNCTION ( Server , Reliable )
	void ServerClientReady ();

	UFUNCTION ( Client , Reliable )
	void ClientPlayingGame ( int32 minutes , int32 seconds )const;

	UFUNCTION ( Client , Reliable )
	void ClientAddportrait ( const FString& PlayerName , UMaterialInstanceDynamic* portrait ) const;
protected:

	bool bClientReady = false;
#pragma endregion


};
