// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TTInGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTInGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void PostInitializeComponents () override;

#pragma region ChatUI
	void AddChat ()const;
	void AddChatMessage ( const FString& Message );

public:
	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UUserWidget> ChatClass;

	UPROPERTY( VisibleAnywhere )
	TObjectPtr<class UTTChatUI> Chat;


	UPROPERTY( EditDefaultsOnly )
	TSubclassOf<UUserWidget> ChatMessageClass;

	UPROPERTY ( VisibleAnywhere )
	TObjectPtr<class ATTPlayerController> OwningPlayer;
#pragma endregion

#pragma region ESCMenu
public:
	void AddESCMenu () const;

public:
	UPROPERTY ( EditDefaultsOnly )
	TSubclassOf<UUserWidget> ESCMenuClass;
	UPROPERTY ( VisibleAnywhere )
	TObjectPtr<class UUserWidget> ESCMenu;
#pragma endregion

#pragma region SelectedSkeletalMesh
public:
	void AddSelectedSkeletalMenu () const;

public:
	UPROPERTY ( EditDefaultsOnly )
	TSubclassOf<UUserWidget> SelectedSkeletalMeshClass;

	UPROPERTY ( VisibleAnywhere )
	TObjectPtr<class UUserWidget> SelectedSkeletalMesh;

#pragma endregion
#pragma region NotificationWidget
public:
	void AddNotification () const;
	void StartAnim ()const;
	void CountDownTimer ( int32 minutes , int32 seconds )const;
public:
	UPROPERTY ( EditDefaultsOnly )
	TSubclassOf<class UTTNotificationWidget> NotificationClass;
	UPROPERTY ( VisibleAnywhere )
	TObjectPtr<UTTNotificationWidget> Notification;
#pragma endregion

#pragma region sound
	UPROPERTY ( EditDefaultsOnly , Category = "Audio" )
	USoundBase* DesertBGM;
#pragma endregion
};
