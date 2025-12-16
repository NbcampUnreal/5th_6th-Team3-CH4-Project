// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Customize.generated.h"

class UButton;
class USkeletalMesh;
struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class TEAM03_API UUW_Customize : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
#pragma region Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Head_Next;
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Head_Prev;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Body_Next;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Body_Prev;
#pragma endregion

private:
#pragma region Internal State
	// 순환 탐색을 위한 인덱스 추적
	int32 CurrentHeadIndex;
	int32 CurrentBodyIndex;

	// 메쉬 변경 적용 헬퍼
	void ApplyHeadChange();
	void ApplyBodyChange();
	
	TSharedPtr<FStreamableHandle> AssetStreamableHandle;
#pragma endregion

#pragma region Callbacks
	UFUNCTION()
	void OnClickClose();

	UFUNCTION()
	void OnClickHeadNext();
    
	UFUNCTION()
	void OnClickHeadPrev();

	UFUNCTION()
	void OnClickBodyNext();

	UFUNCTION()
	void OnClickBodyPrev();
#pragma endregion
};
