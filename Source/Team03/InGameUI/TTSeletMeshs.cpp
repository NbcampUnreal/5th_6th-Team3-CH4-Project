// Fill out your copyright notice in the Description page of Project Settings.


#include "TTSeletMeshs.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"
#include "../Controller/TTPlayerController.h"
#include "Character/TTPlayerState.h"
#include "Components/Button.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UTTSeletMeshs::NativeConstruct ()
{
	Super::NativeConstruct ();

	if ( HeadLeft )
	{
		HeadLeft->OnClicked.AddDynamic ( this , &UTTSeletMeshs::OnHeadLeftClicked );
	}
	if ( HeadRight )
	{
		HeadRight->OnClicked.AddDynamic ( this , &UTTSeletMeshs::OnHeadRightClicked );
	}
	if ( BodyLeft )
	{
		BodyLeft->OnClicked.AddDynamic ( this , &UTTSeletMeshs::OnBodyLeftClicked );
	}
	if ( BodyRight )
	{
		BodyRight->OnClicked.AddDynamic ( this , &UTTSeletMeshs::OnBodyRightClicked );
	}
	if (ExitWidget)
	{
		ExitWidget->OnClicked.AddDynamic ( this , &UTTSeletMeshs::RemoveUI );
	}
	IndexHead = 0;
	IndexBody = 0;
}

void UTTSeletMeshs::OnHeadLeftClicked ()
{

	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect> ();

	if (0 < CDO->PlayerCharacterHeadSkeletalPaths.Num ())
	{
		if ( ++IndexHead >= CDO->PlayerCharacterHeadSkeletalPaths.Num () )
		{
			IndexHead = 0;
		}

		CurrentPlayerHeadSkeletalMeshPath = CDO->PlayerCharacterHeadSkeletalPaths[IndexHead];

		AssetStreamableHandle = UAssetManager::GetStreamableManager ().RequestAsyncLoad (
			{ CurrentPlayerHeadSkeletalMeshPath } ,
			FStreamableDelegate::CreateLambda ( [this]() -> void
				{

					AssetStreamableHandle->ReleaseHandle ();
					TSoftObjectPtr<USkeletalMesh> LoadedInstance ( CurrentPlayerHeadSkeletalMeshPath );
					if (LoadedInstance.IsValid ())
					{

						ATTPlayerController* PC = Cast<ATTPlayerController> ( GetOwningPlayer () );
						if (IsValid ( PC ))
						{
							PC->ServerRequestChangeHeadMesh ( LoadedInstance );
						}
						
					}
				}
			)
		);
	}
}

void UTTSeletMeshs::OnHeadRightClicked ()
{

	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect> ();


	if (0 < CDO->PlayerCharacterHeadSkeletalPaths.Num ())
	{
		if (--IndexHead < 0)
		{
			IndexHead = CDO->PlayerCharacterHeadSkeletalPaths.Num () -1;
		}


		CurrentPlayerHeadSkeletalMeshPath = CDO->PlayerCharacterHeadSkeletalPaths[IndexHead];

		AssetStreamableHandle = UAssetManager::GetStreamableManager ().RequestAsyncLoad (
			{ CurrentPlayerHeadSkeletalMeshPath } ,
			FStreamableDelegate::CreateLambda ( [this]() -> void
				{

					AssetStreamableHandle->ReleaseHandle ();
					TSoftObjectPtr<USkeletalMesh> LoadedInstance ( CurrentPlayerHeadSkeletalMeshPath );
					if (LoadedInstance.IsValid ())
					{

						ATTPlayerController* PC = Cast<ATTPlayerController> ( GetOwningPlayer () );
						if (IsValid ( PC ))
						{
							PC->ServerRequestChangeHeadMesh ( LoadedInstance );
						}
					}
				}
			)
		);

	}
}

void UTTSeletMeshs::OnBodyLeftClicked ()
{
	const UTTCharactorSkeletalMeshSelect* CDO = GetDefault<UTTCharactorSkeletalMeshSelect> ();

	if(0< CDO->PlayerCharacterSkeletalPaths.Num ())
	{
		if ( ++IndexBody >= CDO->PlayerCharacterSkeletalPaths.Num ())
		{
			IndexBody = 0;
		}

		CurrentPlayerSkeletalMeshPath = CDO->PlayerCharacterSkeletalPaths[IndexBody];

		AssetStreamableHandle = UAssetManager::GetStreamableManager ().RequestAsyncLoad (
			{ CurrentPlayerSkeletalMeshPath } ,
			FStreamableDelegate::CreateLambda ( [this]() -> void
				{

					AssetStreamableHandle->ReleaseHandle ();
					TSoftObjectPtr<USkeletalMesh> LoadedInstance ( CurrentPlayerSkeletalMeshPath );
					if (LoadedInstance.IsValid ())
					{

						ATTPlayerController* PC = Cast<ATTPlayerController> ( GetOwningPlayer () );
						if (IsValid ( PC ))
						{
							PC->ServerRequestChangeBodyMesh ( LoadedInstance );
						}
					}
				}
			)
		);
	}
}

void UTTSeletMeshs::OnBodyRightClicked ()
{

	const UTTCharactorSkeletalMeshSelect* CDO = GetDefault<UTTCharactorSkeletalMeshSelect> ();

	if (0 < CDO->PlayerCharacterSkeletalPaths.Num ())
	{
		if ( --IndexBody < 0 )
		{
			IndexBody = CDO->PlayerCharacterSkeletalPaths.Num () - 1;
		}

		CurrentPlayerSkeletalMeshPath = CDO->PlayerCharacterSkeletalPaths[IndexBody];

		AssetStreamableHandle = UAssetManager::GetStreamableManager ().RequestAsyncLoad (
			{ CurrentPlayerSkeletalMeshPath } ,
			FStreamableDelegate::CreateLambda ( [this]() -> void
				{

					AssetStreamableHandle->ReleaseHandle ();
					TSoftObjectPtr<USkeletalMesh> LoadedInstance ( CurrentPlayerSkeletalMeshPath );
					if (LoadedInstance.IsValid ())
					{

						ATTPlayerController* PC = Cast<ATTPlayerController> ( GetOwningPlayer () );
						if (IsValid ( PC ))
						{
							PC->ServerRequestChangeBodyMesh ( LoadedInstance );
						}
					}
				}
			)
		);
	}
}

void UTTSeletMeshs::RemoveUI ()
{
	RemoveFromParent ();
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetOwningPlayer () ))
	{
		PC->SetShowMouseCursor ( false );
		FInputModeGameOnly InputMode;
		PC->SetInputMode ( InputMode );
	}
}
