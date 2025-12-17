// (c) 2024. Team03. All rights reserved.

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
#pragma region Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> Input_Nickname;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Create;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Find;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Option;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> OptionWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> LoadingOverlay;

	// 게임 찾기 오버레이
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
#pragma endregion

#pragma region Callbacks
    // GameInstance 상호작용 헬퍼
	UFUNCTION()
	void OnCreateClicked();

	UFUNCTION()
	void OnFindClicked();

	UFUNCTION()
	void OnCloseOverlayClicked();

	UFUNCTION()
	void OnOptionClicked();

	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnNicknameChanged(const FText& Text);

	UFUNCTION()
	void OnSessionSearchCompleted(bool bWasSuccessful);

	// BP에서 목록을 채우는 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateSessionList(const TArray<FTTSessionInfo>& SessionResults);
#pragma endregion
    
#pragma region UI State
    // UI 상태
	void SetLoadingState(bool bIsLoading);
#pragma endregion
};
