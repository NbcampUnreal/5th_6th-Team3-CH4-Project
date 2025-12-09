// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "TTGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FTTSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 SessionIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Session")
	FString HostName;

	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Session")
	int32 Ping = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindSessionsCompleteBP, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class TEAM03_API UTTGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UTTGameInstance();
	virtual void Init() override;

	// --- 3.1 Data Persistence ---
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	FString UserNickname;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	FName SelectedCharacterRowName;

	// --- Session Management ---
	UFUNCTION(BlueprintCallable, Category = "Session")
	void CreateGameSession(bool bIsLAN);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindGameSessions(bool bIsLAN);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void JoinGameSession(int32 SessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void DestroyGameSession();

	UFUNCTION(BlueprintCallable, Category = "Session")
	TArray<FTTSessionInfo> GetSessionSearchResults() const;

	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnFindSessionsCompleteBP OnFindSessionsCompleteBP;

protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnFindSessionsComplete(bool bWasSuccessful);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
};
