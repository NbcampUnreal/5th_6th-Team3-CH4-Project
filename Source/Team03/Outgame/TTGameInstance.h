// (c) 2024. Team03. All rights reserved.

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

#pragma region Data Persistence
	// 데이터 지속성
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	FString UserNickname;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	FName SelectedCharacterRowName;
	
	// 로비 커스터마이징 데이터 (로컬 플레이어 전용)
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	int32 CustomizedHeadIndex = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	int32 CustomizedBodyIndex = 0;
#pragma endregion

#pragma region Session Management
	// 세션 관리
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
#pragma endregion

protected:
#pragma region Session Callbacks
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnFindSessionsComplete(bool bWasSuccessful);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    // 헬퍼: 새 세션 참여 전 세션 정리
	void OnDestroySessionBeforeJoin(FName SessionName, bool bWasSuccessful);
#pragma endregion

private:
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
    
    // 정리 후 참여할 세션 인덱스
    int32 PendingJoinSessionIndex = -1;

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
};
