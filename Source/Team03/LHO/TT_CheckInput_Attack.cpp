// Fill out your copyright notice in the Description page of Project Settings.


#include "LHO/TT_CheckInput_Attack.h"

#include "Character/TTPlayerCharacter.h"

void UTT_CheckInput_Attack::Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference )
{
	Super::Notify ( MeshComp , Animation , EventReference );

	if (IsValid ( MeshComp ) == true)
	{
		ATTPlayerCharacter* AttackingCharacter = Cast<ATTPlayerCharacter> ( MeshComp->GetOwner () );
		if (IsValid ( AttackingCharacter ) == true)
		{
			AttackingCharacter->HandleOnCheckInputAttack ();
		}
	}
}