//Slow_Glass.cpp

#include "Slow_Glass.h"
#include "Character/TTPlayerCharacter.h"

ASlow_Glass::ASlow_Glass ()
{
	SlowAmount = 0.5f;
	SlowDuration = 3.f;

	SlowTrigger = CreateDefaultSubobject<USphereComponent> ( TEXT ( "SlowTrigger" ) );
	SlowTrigger->SetupAttachment ( RootComponent );
	SlowTrigger->SetSphereRadius ( 200.f );
	SlowTrigger->SetCollisionEnabled ( ECollisionEnabled::QueryOnly );
	SlowTrigger->SetCollisionResponseToAllChannels ( ECR_Ignore );
	SlowTrigger->SetCollisionResponseToChannel ( ECC_Pawn , ECR_Overlap );

	SlowTrigger->SetGenerateOverlapEvents ( false );
}

void ASlow_Glass::BeginPlay ()
{
	Super::BeginPlay ();

	if (!HasAuthority ()) return;

	SlowTrigger->OnComponentBeginOverlap.AddDynamic (
		this ,
		&ASlow_Glass::OnSlowOverlap
	);

	FTimerHandle ArmTimer;
	GetWorld ()->GetTimerManager ().SetTimer (
		ArmTimer ,
		[this]()
		{
			bArmed = true;
			SlowTrigger->SetGenerateOverlapEvents ( true );
		} ,
		0.3f ,
		false
	);
}

void ASlow_Glass::OnSlowOverlap (
	UPrimitiveComponent* OverlappedComp ,
	AActor* OtherActor ,
	UPrimitiveComponent* OtherComp ,
	int32 OtherBodyIndex ,
	bool bFromSweep ,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority ())
	{
		return;
	}

	if (!bArmed || bIgnoreInitialOverlap)
	{
		return;
	}

	if (ATTPlayerCharacter* Player = Cast<ATTPlayerCharacter> ( OtherActor ))
	{
		Player->ApplySlow ( SlowAmount , SlowDuration );

		Explode ();
	}
}