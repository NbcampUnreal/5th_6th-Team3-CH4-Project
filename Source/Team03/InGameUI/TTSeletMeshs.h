// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTSeletMeshs.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTTSeletMeshs : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct () override;

	UFUNCTION ()
	void OnHeadLeftClicked ();

	UFUNCTION ()
	void OnHeadRightClicked ();

	UFUNCTION ()
	void OnBodyLeftClicked ();

	UFUNCTION ()
	void OnBodyRightClicked ();

	UFUNCTION ()
	void RemoveUI ();
private:


	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UButton> HeadLeft;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UButton> HeadRight;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UButton> BodyLeft;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UButton> BodyRight;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UButton> ExitWidget;

	FSoftObjectPath CurrentPlayerSkeletalMeshPath = FSoftObjectPath ();

	FSoftObjectPath CurrentPlayerHeadSkeletalMeshPath = FSoftObjectPath ();

	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;

	int32 IndexHead;

	int32 IndexBody;
};
