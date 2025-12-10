// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTWeaponBase.h"
#include "TTSword.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTSword : public ATTWeaponBase
{
	GENERATED_BODY()
public:
	ATTSword ();
	void BeginPlay () override;
	virtual void Attack () override;
	void AttackEnd () override;

private:
	UPROPERTY ( EditAnywhere )
	TObjectPtr<UStaticMeshComponent> SwordMesh;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Combat" , meta = (AllowPrivateAccess = "true") )
	TObjectPtr<class UBoxComponent> MeleeCollisionBox;

	/** 히트박스에 오버랩 감지 시 호출될 함수 */
	UFUNCTION ()
	void OnMeleeOverlap ( UPrimitiveComponent* OverlappedComponent ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		int32 OtherBodyIndex ,
		bool bFromSweep ,
		const FHitResult& SweepResult );
	/** 공격에 이미 감지된 캐릭터 리스트 (중복 히트 방지용) */
	TArray<class ACharacter*> HitCharacters;

};
