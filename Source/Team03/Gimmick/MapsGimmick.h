//MapsGimmick.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapsGimmick.generated.h"

class UBoxComponent;

UCLASS ()
class TEAM03_API AMapsGimmick : public AActor
{
	GENERATED_BODY ()

public:
	AMapsGimmick ();

protected:
	virtual void BeginPlay () override;

	//가스 데미지
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly, Category = "Gas Gimmick" )
	class UBoxComponent* GasDetectionVolume;

	FTimerHandle GasDamageTimerHandle; 

	UPROPERTY ( EditAnywhere , Category = "Gas Danage Time" )
	float DamageInterval = 1.0f;

	UPROPERTY ( EditAnywhere , Category = "Gas Damage" )
	float DamagePerTick = 10.0f;

	void GasDamage ();

	//게임 시작후 가스 방출
	UPROPERTY ( Replicated )
	bool bGasActive = false;

	FTimerHandle GasStartTimer;

	UFUNCTION ()
	void StartGasDamage ();

	//가스 안에 캐릭터가 들어오면 시작
	UFUNCTION ()
	void OnOverlapBegin (
		UPrimitiveComponent* OverlappedComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		int32 OtherBodyIndex ,
		bool bFromSweep ,
		const FHitResult& SweepResult );

	UFUNCTION ()
	void OnOverlapEnd (
		UPrimitiveComponent* OverlappedComp ,
		AActor* OtherActor ,
		UPrimitiveComponent* OtherComp ,
		int32 OtherBodyIndex );

	TArray<ACharacter*> ActorsInGas;

	//독가스 크기
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Gas Gimmick" )
	FVector GasBoxExtent = FVector ( 1000.f , 1000.f , 30.f );

	//가스 시작 딜레이
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Gas Stats" )
	float GasStartDelay = 10.f;

public:
	virtual void Tick ( float DeltaTime ) override;

	virtual void GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

};
