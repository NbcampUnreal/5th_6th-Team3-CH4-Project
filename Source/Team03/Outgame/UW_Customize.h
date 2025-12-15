// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Customize.generated.h"

class UButton;
class USkeletalMesh;
struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_Customize : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Head_Next;
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Head_Prev;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Body_Next;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Body_Prev;

private:
	// Index tracking for cyclic navigation
	int32 CurrentHeadIndex;
	int32 CurrentBodyIndex;

	// Helper to apply mesh change
	void ApplyHeadChange();
	void ApplyBodyChange();
	
	TSharedPtr<FStreamableHandle> AssetStreamableHandle;

	UFUNCTION()
	void OnClickClose();

	UFUNCTION()
	void OnClickHeadNext();
    
	UFUNCTION()
	void OnClickHeadPrev();

	UFUNCTION()
	void OnClickBodyNext();

	UFUNCTION()
	void OnClickBodyPrev();
};
