// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTGameInstance.h"
#include "UW_LobbyEntry.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_LobbyEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void Setup(const FTTSessionInfo& InInfo);

protected:
#pragma region Widgets
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_HostName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_PlayerCount;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Ping;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Join;
#pragma endregion

private:
#pragma region Callbacks
	UFUNCTION()
	void OnJoinClicked();
#pragma endregion

	FTTSessionInfo SessionInfo;
};
