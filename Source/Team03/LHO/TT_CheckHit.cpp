// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TT_CheckHit.h"
#include "Character/TTPlayerCharacter.h"
#include "LHO/TTAnimInstance.h"

void UTT_CheckHit::Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation ,
	const FAnimNotifyEventReference& EventReference )
{
	Super::Notify ( MeshComp , Animation , EventReference );

	if (IsValid ( MeshComp ) == true)
	{
		ATTPlayerCharacter* AttackingCharacter = Cast<ATTPlayerCharacter> ( MeshComp->GetOwner () );
		if (IsValid ( AttackingCharacter ) == true)
		{
			AttackingCharacter->HandleOnCheckHit ();
		}
	}
}