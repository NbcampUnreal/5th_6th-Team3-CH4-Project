// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyLevel.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_LobbyLevel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
#pragma region Widgets
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StartGame;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Leave;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Customize;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CustomizeWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox_PlayerList;
#pragma endregion

private:
#pragma region Callbacks
	UFUNCTION()
	void OnClickStartGame();

	UFUNCTION()
	void OnClickLeave();

	UFUNCTION()
	void OnClickCustomize();

	void UpdatePlayerList();
	FTimerHandle PlayerListTimerHandle;
#pragma endregion
};
