#include "TTWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ATTWeaponBase::ATTWeaponBase () :
	WeaponDamage ( 10 )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent> ( TEXT ( "SceneComp" ) );
	RootComponent = SceneComp;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "WeaponMesh" ) );
	WeaponMesh->SetupAttachment ( SceneComp );
}

void ATTWeaponBase::BeginPlay ()
{
	Super::BeginPlay ();
}
void ATTWeaponBase::Attack ()
{
	// 공통 구현사항 있다면 넣을것
}
void ATTWeaponBase::AttackEnd ()
{
	// 공통 구현사항 있다면 넣을것
}

