// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
#pragma region Game Loop
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Server RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_StartGame();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_InitPlayerInfo(const FString& Nickname, const FName& CharacterRowName);
#pragma endregion
};
