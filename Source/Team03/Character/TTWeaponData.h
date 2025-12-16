#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TTWeaponData.generated.h"

USTRUCT(BlueprintType)
struct FTTWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FTTWeaponData () :
		StunAmount ( 0 ) ,
		KnockbackAmount ( 0 ) ,
		AttackDelay ( 0 )
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	float StunAmount;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "WeaponData" )
	float KnockbackAmount;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "WeaponData" )
	float AttackDelay;

};
