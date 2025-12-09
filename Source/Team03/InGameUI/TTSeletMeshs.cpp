// Fill out your copyright notice in the Description page of Project Settings.


#include "TTSeletMeshs.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"
#include "../Controller/TTPlayerController.h"
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

	IndexHead = 0;
	IndexBody = 0;
}

void UTTSeletMeshs::OnHeadLeftClicked ()
{
	const UTTCharactorHeadSkeletalSelect* HeadSkeletalSelect = GetDefault<UTTCharactorHeadSkeletalSelect> ();
	if (0 < HeadSkeletalSelect->DefaultPlayerCharacterHeadSkeletalPaths.Num ())
	{
		if ( ++IndexHead >= HeadSkeletalSelect->DefaultPlayerCharacterHeadSkeletalPaths.Num () )
		{
			IndexHead = 0;
		}
		CurrentPlayerHeadSkeletalMeshPath = HeadSkeletalSelect->DefaultPlayerCharacterHeadSkeletalPaths[IndexHead];

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
							PC->ChangeHeadMesh (LoadedInstance.Get () );
						}
						
					}
				}
			)
		);
	}
}

void UTTSeletMeshs::OnHeadRightClicked ()
{
	const UTTCharactorHeadSkeletalSelect* HeadSkeletalSelect = GetDefault<UTTCharactorHeadSkeletalSelect> ();
	if (0 < HeadSkeletalSelect->DefaultPlayerCharacterHeadSkeletalPaths.Num ())
	{
		if (--IndexHead < 0)
		{
			IndexHead = HeadSkeletalSelect->DefaultPlayerCharacterHeadSkeletalPaths.Num () -1;
		}
		CurrentPlayerHeadSkeletalMeshPath = HeadSkeletalSelect->DefaultPlayerCharacterHeadSkeletalPaths[IndexHead];

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
							PC->ChangeHeadMesh ( LoadedInstance.Get () );
						}
					}
				}
			)
		);
	}
}

void UTTSeletMeshs::OnBodyLeftClicked ()
{
	const UTTCharactorSkeletalMeshSelect* BodySkeletalSelect = GetDefault<UTTCharactorSkeletalMeshSelect> ();
	if(0< BodySkeletalSelect->DefaultPlayerCharacterSkeletalPaths.Num ())
	{
		if ( ++IndexBody >= BodySkeletalSelect->DefaultPlayerCharacterSkeletalPaths.Num ())
		{
			IndexBody = 0;
		}

		CurrentPlayerSkeletalMeshPath = BodySkeletalSelect->DefaultPlayerCharacterSkeletalPaths[IndexBody];

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
							PC->ChangeMesh ( LoadedInstance.Get () );
						}
					}
				}
			)
		);

	}
}

void UTTSeletMeshs::OnBodyRightClicked ()
{
	const UTTCharactorSkeletalMeshSelect* BodySkeletalSelect = GetDefault<UTTCharactorSkeletalMeshSelect> ();
	if (0 < BodySkeletalSelect->DefaultPlayerCharacterSkeletalPaths.Num ())
	{
		if ( --IndexBody < 0 )
		{
			IndexBody = BodySkeletalSelect->DefaultPlayerCharacterSkeletalPaths.Num () - 1;
		}

		CurrentPlayerSkeletalMeshPath = BodySkeletalSelect->DefaultPlayerCharacterSkeletalPaths[IndexBody];

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
							PC->ChangeMesh ( LoadedInstance.Get () );
						}
					}
				}
			)
		);
	}
}
