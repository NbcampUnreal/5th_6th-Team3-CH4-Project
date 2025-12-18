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
    
    // 시작 메뉴 컨테이너 (커스터마이즈 시 숨김)
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* VerticalBox_Start;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CustomizeWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox_PlayerListRedTeam;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox_PlayerListBlueTeam;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_RedTeam;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_BlueTeam;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* LobbyBGM;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ClickSound;

#pragma endregion

private:
#pragma region Callbacks
	UFUNCTION()
	void OnClickStartGame();

	UFUNCTION()
	void OnClickLeave();

	UFUNCTION()
	void OnClickCustomize();
    
    UFUNCTION()
    void OnCustomizeClosedCallback();

	UFUNCTION()
	void OnClickRedTeam();

	UFUNCTION()
	void OnClickBlueTeam();

	void UpdatePlayerList();

	FTimerHandle PlayerListTimerHandle;
#pragma endregion
};
