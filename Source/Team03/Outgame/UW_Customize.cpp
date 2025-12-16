// (c) 2024. Team03. All rights reserved.

#include "UW_Customize.h"
#include "Components/Button.h"
#include "TTLobbyPlayerController.h"
#include "Character/TTLobbyCharacter.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UUW_Customize::NativeConstruct()
{
	Super::NativeConstruct();

	// 캐릭터 현재 상태에서 인덱스 초기화
	if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(GetOwningPlayer()))
	{
		if (ATTLobbyCharacter* Character = Cast<ATTLobbyCharacter>(PC->GetPawn()))
		{
			CurrentHeadIndex = Character->GetCurrentHeadIndex();
			CurrentBodyIndex = Character->GetCurrentBodyIndex();
		}
		else
		{
			// 캐릭터를 아직 사용할 수 없는 경우 대비
			CurrentHeadIndex = 0;
			CurrentBodyIndex = 0;
		}
	}

	// 버튼 콜백 바인딩
	if (Btn_Close) Btn_Close->OnClicked.AddDynamic(this, &ThisClass::OnClickClose);
	if (Btn_Head_Next) Btn_Head_Next->OnClicked.AddDynamic(this, &ThisClass::OnClickHeadNext);
	if (Btn_Head_Prev) Btn_Head_Prev->OnClicked.AddDynamic(this, &ThisClass::OnClickHeadPrev);
	if (Btn_Body_Next) Btn_Body_Next->OnClicked.AddDynamic(this, &ThisClass::OnClickBodyNext);
	if (Btn_Body_Prev) Btn_Body_Prev->OnClicked.AddDynamic(this, &ThisClass::OnClickBodyPrev);
}

#pragma region Callbacks

void UUW_Customize::OnClickClose()
{
	RemoveFromParent();
    
    // 이상적으로는 로비 위젯으로 포커스를 복구해야 하지만, 단순 제거로 처리
    if (APlayerController* PC = GetOwningPlayer())
    {
         FInputModeGameAndUI InputMode;
         InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
         PC->SetInputMode(InputMode);
         PC->SetShowMouseCursor(true);
    }
}

void UUW_Customize::OnClickHeadNext()
{
	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect>();
	if (CDO && CDO->PlayerCharacterHeadSkeletalPaths.Num() > 0)
	{
		CurrentHeadIndex = (CurrentHeadIndex + 1) % CDO->PlayerCharacterHeadSkeletalPaths.Num();
		ApplyHeadChange();
	}
}

void UUW_Customize::OnClickHeadPrev()
{
	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect>();
	if (CDO && CDO->PlayerCharacterHeadSkeletalPaths.Num() > 0)
	{
		CurrentHeadIndex--;
		if (CurrentHeadIndex < 0)
		{
			CurrentHeadIndex = CDO->PlayerCharacterHeadSkeletalPaths.Num() - 1;
		}
		ApplyHeadChange();
	}
}

void UUW_Customize::OnClickBodyNext()
{
	const UTTCharactorSkeletalMeshSelect* CDO = GetDefault<UTTCharactorSkeletalMeshSelect>();
	if (CDO && CDO->PlayerCharacterSkeletalPaths.Num() > 0)
	{
		CurrentBodyIndex = (CurrentBodyIndex + 1) % CDO->PlayerCharacterSkeletalPaths.Num();
		ApplyBodyChange();
	}
}

void UUW_Customize::OnClickBodyPrev()
{
	const UTTCharactorSkeletalMeshSelect* CDO = GetDefault<UTTCharactorSkeletalMeshSelect>();
	if (CDO && CDO->PlayerCharacterSkeletalPaths.Num() > 0)
	{
		CurrentBodyIndex--;
		if (CurrentBodyIndex < 0)
		{
			CurrentBodyIndex = CDO->PlayerCharacterSkeletalPaths.Num() - 1;
		}
		ApplyBodyChange();
	}
}

#pragma endregion

#pragma region Helpers

void UUW_Customize::ApplyHeadChange()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	ATTLobbyCharacter* Character = Cast<ATTLobbyCharacter>(PC->GetPawn());
	if (!Character)
	{
		return;
	}
	
	Character->ChangeHead(CurrentHeadIndex);
}

void UUW_Customize::ApplyBodyChange()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	ATTLobbyCharacter* Character = Cast<ATTLobbyCharacter>(PC->GetPawn());
	if (!Character)
	{
		return;
	}
	
	Character->ChangeBody(CurrentBodyIndex);
}

#pragma endregion
