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
	virtual void Logout(AController* Exiting) override;

	void StartGame();
#pragma endregion

};
