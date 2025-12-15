// Fill out your copyright notice in the Description page of Project Settings.


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

	// Initialize indices from character's current state
	if (ATTLobbyPlayerController* PC = Cast<ATTLobbyPlayerController>(GetOwningPlayer()))
	{
		if (ATTLobbyCharacter* Character = Cast<ATTLobbyCharacter>(PC->GetPawn()))
		{
			CurrentHeadIndex = Character->GetCurrentHeadIndex();
			CurrentBodyIndex = Character->GetCurrentBodyIndex();
		}
		else
		{
			// Fallback if character not available yet
			CurrentHeadIndex = 0;
			CurrentBodyIndex = 0;
		}
	}

	// Bind button callbacks
	if (Btn_Close) Btn_Close->OnClicked.AddDynamic(this, &ThisClass::OnClickClose);
	if (Btn_Head_Next) Btn_Head_Next->OnClicked.AddDynamic(this, &ThisClass::OnClickHeadNext);
	if (Btn_Head_Prev) Btn_Head_Prev->OnClicked.AddDynamic(this, &ThisClass::OnClickHeadPrev);
	if (Btn_Body_Next) Btn_Body_Next->OnClicked.AddDynamic(this, &ThisClass::OnClickBodyNext);
	if (Btn_Body_Prev) Btn_Body_Prev->OnClicked.AddDynamic(this, &ThisClass::OnClickBodyPrev);
}

void UUW_Customize::OnClickClose()
{
	RemoveFromParent();
    
    // Restore Focus to Lobby Widget ideally, but simpler is just remove.
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
