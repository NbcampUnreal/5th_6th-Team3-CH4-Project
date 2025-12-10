// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTWeaponBase.generated.h"

UCLASS()
class TEAM03_API ATTWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	ATTWeaponBase ();

protected:
	virtual void BeginPlay () override;

public:
	virtual void Attack ();
	virtual void AttackEnd ();
	TObjectPtr<class USceneComponent> SceneComp;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "WeaponSetting" )
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh;

	int32 WeaponDamage;
};
