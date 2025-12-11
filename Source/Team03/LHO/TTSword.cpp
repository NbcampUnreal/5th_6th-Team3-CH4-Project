//TTSword.cpp

#include "TTSword.h"
#include "Components/BoxComponent.h"
#include "../Character/TTPlayerCharacter.h"
#include "GameFramework/Character.h"

ATTSword::ATTSword ()
{
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "SwordMesh" ) );
	MeleeCollisionBox = CreateDefaultSubobject<UBoxComponent> ( TEXT ( "MeleeCollisionBox" ) );

	SwordMesh->SetupAttachment ( RootComponent );
	MeleeCollisionBox->SetupAttachment ( SwordMesh );


	MeleeCollisionBox->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	MeleeCollisionBox->SetCollisionObjectType ( ECollisionChannel::ECC_WorldDynamic );
	MeleeCollisionBox->SetCollisionResponseToAllChannels ( ECollisionResponse::ECR_Ignore );                            // 모든 채널 무시
	MeleeCollisionBox->SetCollisionResponseToChannel ( ECollisionChannel::ECC_Pawn , ECollisionResponse::ECR_Overlap ); // Pawn 채널과 오버랩 감지

	this->WeaponDamage = 10;
}

void ATTSword::BeginPlay ()
{
	Super::BeginPlay ();

	if (MeleeCollisionBox)
	{
		MeleeCollisionBox->OnComponentBeginOverlap.AddDynamic ( this , &ATTSword::OnMeleeOverlap );
	}
}

void ATTSword::Attack ()
{
	Super::Attack ();

	UE_LOG(LogTemp, Warning, TEXT("Default Sword Attack"));  //실험용

	MeleeCollisionBox->SetCollisionEnabled ( ECollisionEnabled::QueryOnly );
	HitCharacters.Empty ();

}

void ATTSword::AttackEnd ()
{

	Super::AttackEnd ();

	//UE_LOG(LogTemp, Warning, TEXT("Default Sword Attack End"));
	// 공격이 끝나면 콜리전 없앰
	MeleeCollisionBox->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
}

void ATTSword::OnMeleeOverlap ( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult )
{
	//오버랩된 액터가 유효하고 자기 자신이 아닌지 확인
	if (OtherActor && OtherActor != this)
	{
		//Character에게 캐스팅
		ACharacter* Character = Cast<ACharacter> ( OtherActor );
		//캐스팅 성공 시
		if (Character)
		{
			//중복 타격 방지
			if (!HitCharacters.Contains ( Character ))
			{
				HitCharacters.Add ( Character );

				ACharacter* weaponOwner = Cast<ACharacter> ( GetOwner ());
				if (!weaponOwner)
				{
					UE_LOG ( LogTemp , Warning , TEXT ( "Weapon Owner is invalid." ) );
					return;
				}
				//데미지
				float Damage = 10.0f;
				//넉백
				FVector LaunchDir = GetActorForwardVector () * 300.f;
				Character->LaunchCharacter ( LaunchDir , true , true );
			}
		}
	}
}