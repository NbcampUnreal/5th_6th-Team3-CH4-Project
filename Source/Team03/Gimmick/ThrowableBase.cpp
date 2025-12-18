// ThrowableBase.cpp

#include "ThrowableBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AThrowableBase::AThrowableBase ()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "MeshComp" ) );
	RootComponent = MeshComp;

	MeshComp->SetSimulatePhysics ( true );
	MeshComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
	MeshComp->SetCollisionObjectType ( ECC_PhysicsBody );

	MeshComp->SetNotifyRigidBodyCollision(true);
	MeshComp->SetGenerateOverlapEvents(false);

	bReplicates = true;
	SetReplicateMovement ( true );

	FuseTime;
	Damage;
	DamageRadius;

	bExploded = false;
}

void AThrowableBase::BeginPlay ()
{
	Super::BeginPlay ();

	if (MeshComp)
	{
		MeshComp->OnComponentHit.AddDynamic(this, &AThrowableBase::OnHit);
	}

	if (HasAuthority ())
	{
		GetWorld ()->GetTimerManager ().SetTimer (
			FuseTimerHandle ,
			this ,
			&AThrowableBase::Explode ,
			FuseTime ,
			false
		);
	}
}

void AThrowableBase::GetLifetimeReplicatedProps ( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );

	DOREPLIFETIME ( AThrowableBase , bExploded );
}

void AThrowableBase::OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bExploded)
	{
		return;
	}

	bExploded = true;

	Multicast_ExplodeEffects();
	Destruct();       
}


void AThrowableBase::ServerThrow_Implementation ( const FVector& Direction , float Power )
{
	Throw ( Direction , Power );
}

void AThrowableBase::Throw ( const FVector& Direction , float Power )
{
	if (!MeshComp) return;

	MeshComp->AddImpulse ( Direction * Power , NAME_None , true );
}

void AThrowableBase::Explode_Implementation ()
{
	if (bExploded) return;

	bExploded = true;

	if (HasAuthority ())
	{
		UGameplayStatics::ApplyRadialDamage (
			this ,
			Damage ,
			GetActorLocation () ,
			DamageRadius ,
			nullptr ,
			TArray<AActor*> () ,
			this ,
			GetInstigatorController () ,
			true
		);

		Multicast_ExplodeEffects ();
	}

	Destruct ();
}

void AThrowableBase::Multicast_ExplodeEffects_Implementation ()
{
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation (
			GetWorld () ,
			ExplosionParticle ,
			GetActorTransform ()
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation (
			this ,
			ExplosionSound ,
			GetActorLocation ()
		);
	}
}


void AThrowableBase::Destruct_Implementation ()
{
	if (HasAuthority ())
	{
		Destroy ();
	}
}
