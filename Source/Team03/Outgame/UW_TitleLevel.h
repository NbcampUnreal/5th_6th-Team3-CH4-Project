// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTGameInstance.h"
#include "UW_TitleLevel.generated.h"

class UButton;
class UEditableText;
class UScrollBox;

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_TitleLevel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> Input_Nickname;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Create;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Find;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> LoadingOverlay;

	// --- Find Game Overlay ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> Widget_SessionOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_SessionList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_CloseOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_NoSessions;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUW_LobbyEntry> LobbyEntryClass;

    // Helper to interact with GameInstance
	UFUNCTION()
	void OnCreateClicked();

	UFUNCTION()
	void OnFindClicked();

	UFUNCTION()
	void OnCloseOverlayClicked();

	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnNicknameChanged(const FText& Text);

	UFUNCTION()
	void OnSessionSearchCompleted(bool bWasSuccessful);

	// Function to populate list in BP
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateSessionList(const TArray<FTTSessionInfo>& SessionResults);
    
    // UI State
	void SetLoadingState(bool bIsLoading);
};
