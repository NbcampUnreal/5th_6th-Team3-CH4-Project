// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TT_CheckInput_Attack.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UTT_CheckInput_Attack : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	virtual void Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference ) override;

};
