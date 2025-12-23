// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTLobbyPlayerController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLobbyChatMessageReceived, const FString&, Nickname, const FString&, Message, uint8, TeamId);

UCLASS()
class TEAM03_API ATTLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Lobby Chat")
	FOnLobbyChatMessageReceived OnChatMessageReceived;
#pragma region Game Loop
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Server RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_StartGame();

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_ToggleReady();
    
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_InitPlayerInfo(const FString& Nickname, const FName& CharacterRowName);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_SendChatMessage(const FString& Message);
#pragma endregion

#pragma region Client RPC
	UFUNCTION(Client, Reliable)
	void ClientRPC_ReceiveChatMessage(const FString& Nickname, const FString& Message, uint8 TeamId);
#pragma endregion

#pragma region Server RPC Team
	UFUNCTION ( Server , Reliable )
	void Server_RequestChangeTeam ( enum Teams NewTeam );
#pragma endregion
};
