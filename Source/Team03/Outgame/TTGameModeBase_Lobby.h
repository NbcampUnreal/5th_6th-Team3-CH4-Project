// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTGameModeBase_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTGameModeBase_Lobby : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATTGameModeBase_Lobby();

#pragma region Game Loop
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override; // 게임 시작 (Start 버튼 클릭 시 호출)
	void StartGame();

private:
	void ProcessServerTravel();
	FTimerHandle TravelTimerHandle;
#pragma endregion

};
