// ThrowableBase.cpp

#include "ThrowableBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Character/TTPlayerCharacter.h"
#include "LHO/TTPickupComponent.h"

AThrowableBase::AThrowableBase ()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "MeshComp" ) );
	RootComponent = MeshComp;

	MeshComp->SetSimulatePhysics ( true );
	MeshComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
	MeshComp->SetCollisionObjectType ( ECC_PhysicsBody );
	MeshComp->SetNotifyRigidBodyCollision ( true );
	MeshComp->SetGenerateOverlapEvents ( false );

	PickupComponent = CreateDefaultSubobject<UTTPickupComponent> ( TEXT ( "PickupComponent" ) );

	bReplicates = true;
	SetReplicateMovement ( true );

	bDestroyed = false;
	bAllowPickUp = false;
}

void AThrowableBase::BeginPlay ()
{
	Super::BeginPlay ();

	UE_LOG ( LogTemp , Warning , TEXT ( "%s : BeginPlay 호출됨" ) , *GetName () );

	if (MeshComp)
	{
		MeshComp->OnComponentHit.AddDynamic (
			this ,
			&AThrowableBase::OnHit
		);
	}

	if (PickupComponent)
	{
		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	}
}

void AThrowableBase::GetLifetimeReplicatedProps (TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( AThrowableBase , bDestroyed );

	DOREPLIFETIME ( AThrowableBase , bAllowPickUp );

}

void AThrowableBase::HandleOnPickUp ( ATTPlayerCharacter* InPickUpCharacter )
{
	if (!InPickUpCharacter) return;

	if (!HasAuthority ()) return;

	if (!bAllowPickUp) return;
	
	if (OwnerCharacter) return;

	OwnerCharacter = InPickUpCharacter;
	SetOwner ( InPickUpCharacter );

	SetActorHiddenInGame ( true );
	SetActorEnableCollision ( false );

	InPickUpCharacter->AddThrowable ( this );
}

void AThrowableBase::EndPlay ( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay ( EndPlayReason );

	FString Reason;
	switch (EndPlayReason)
	{
	case EEndPlayReason::Destroyed:
		Reason = TEXT ( "Destroyed (코드로 인한 파괴)" );
		break;
	case EEndPlayReason::LevelTransition:
		Reason = TEXT ( "Level Transition (레벨 이동)" );
		break;
	case EEndPlayReason::RemovedFromWorld:
		Reason = TEXT ( "Removed From World (월드에서 제거됨)" );
		break;
	default:
		Reason = TEXT ( "Other Reason" );
		break;
	}

	// 로그 출력 (빨간색으로 표시됨)
	UE_LOG ( LogTemp , Error , TEXT ( "==== [%s] 액터가 사라짐! 사유: %s ====" ) , *GetName () , *Reason );
}

void AThrowableBase::OnHit (UPrimitiveComponent* HitComp ,AActor* OtherActor ,UPrimitiveComponent* OtherComp ,FVector NormalImpulse ,const FHitResult& Hit)
{

}

void AThrowableBase::ServerThrow_Implementation (const FVector& Direction ,float Power)
{
	Throw ( Direction , Power );
}

void AThrowableBase::Throw (const FVector& Direction ,float Power)
{
	if (!MeshComp) return;

	bAllowPickUp = true;

	if (PickupComponent)
	{
		PickupComponent->SetCollisionEnabled ( ECollisionEnabled::QueryOnly );
	}

	SetActorHiddenInGame ( false );
	SetActorEnableCollision ( true );

	MeshComp->AddImpulse ( Direction * Power , NAME_None , true );
}

void AThrowableBase::Explode_Implementation ()
{

}

void AThrowableBase::Destruct_Implementation ()
{
	if (bDestroyed) return;

	bDestroyed = true;

	if (HasAuthority ())
	{
		Destroy ();
	}
}
