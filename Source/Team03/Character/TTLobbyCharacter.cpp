// (c) 2024. Team03. All rights reserved.

#include "Character/TTLobbyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "Character/TTPlayerState.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Outgame/TTGameInstance.h"

// Sets default values
ATTLobbyCharacter::ATTLobbyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 헤드 생성 및 부착
	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(GetMesh());
	Head->SetIsReplicated(true);

    // 닉네임 위젯
    NicknameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NicknameWidget"));
    NicknameWidget->SetupAttachment(GetMesh());
    NicknameWidget->SetWidgetSpace(EWidgetSpace::Screen); // 화면 방향 고정
    NicknameWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f)); // 머리 위
    NicknameWidget->SetDrawSize(FVector2D(300.f, 50.f));

    // 팀 인디케이터 (바닥 원반)
    TeamIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeamIndicatorMesh"));
    TeamIndicatorMesh->SetupAttachment(GetRootComponent());
    TeamIndicatorMesh->SetRelativeLocation(FVector(0.f, 0.f, -85.f)); // 바닥에 붙게 조정 (Capsule HalfHeight 고려)
    TeamIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 없음
    TeamIndicatorMesh->SetCastShadow(false); // 그림자 없음

	// 바디는 기본 Mesh를 사용한다고 가정

	// 바디는 기본 Mesh를 사용한다고 가정

	// 인덱스 초기화
	CurrentHeadIndex = 0;
	CurrentBodyIndex = 0;

	bReplicates = true;
}

#pragma region Life Cycle

// Called when the game starts or when spawned
void ATTLobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATTLobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // PlayerState 정보 동기화 (간단한 구현을 위해 Tick에서 체크)
    if (ATTPlayerState* PS = GetPlayerState<ATTPlayerState>())
    {
        // 닉네임 업데이트
        if (NicknameWidget)
        {
             // 매프레임 텍스트 설정은 비효율적이지만, 로비이므로 일단 적용. 추후 최적화 가능.
             // 위젯 컴포넌트 내부 위젯이 UTextBlock 등을 가지고 있다고 가정하거나, 
             // UWidgetComponent의 SetText 기능(없음) 대신, UserWidget을 Cast해서 써야 함.
             // 여기서는 간단히 Material이나 TextRender가 아닌 WidgetComponent이므로,
             // 1. UserWidget 클래스 지정 필요 -> BP에서 할당 권장.
             // 2. 혹은 Native WidgetComponent 사용.
             
             // 빠른 구현을 위해: WidgetComponent가 UUserWidget을 띄우고 있다고 가정하고 그 안의 TextBlock을 찾아야 함.
             // 하지만 코드로만 해결하려면 TextRenderComponent가 더 쉬울 수 있음.
             // 유저 요청: "캐릭터 위에 닉네임". WidgetComponent가 가장 깔끔함.
             // BP_LobbyCharacter에서 위젯 클래스를 설정해줄 것이라 가정하고, 여기서는 생략하거나, 
             // 간단하게 디버그용으로 TextRender를 쓸 수도 있음.
             // 하지만 User Visuals이므로 Widget이 맞음.
             
             // 여기서는 PS의 정보를 가져올 수 있다는 점만 확인하고, 실제 텍스트 변경은 
             // 위젯 클래스 내부에서 Tick이나 바인딩으로 처리하는게 정석.
             // 하지만 C++에서 강제하려면:
             if (UUserWidget* WidgetObject = NicknameWidget->GetUserWidgetObject())
             {
                 if (UTextBlock* TextBlock = Cast<UTextBlock>(WidgetObject->GetWidgetFromName(TEXT("NicknameText"))))
                 {
                     // 텍스트 업데이트
                     if (TextBlock->GetText().ToString() != PS->UserNickname)
                     {
                         TextBlock->SetText(FText::FromString(PS->UserNickname));
                     }
                     
                     // 색상 업데이트
                     FLinearColor TextColor = FLinearColor::White;
                     if (PS->GetTeam() == Teams::Red) TextColor = FLinearColor::Red;
                     else if (PS->GetTeam() == Teams::Blue) TextColor = FLinearColor::Blue;
                     
                     TextBlock->SetColorAndOpacity(FSlateColor(TextColor));
                 }
             }
        }

        // 팀 인디케이터 색상 업데이트
        if (TeamIndicatorMesh)
        {
            // DMI 생성 (Lazy Init)
            if (!IndicatorMID && TeamIndicatorMesh->GetMaterial(0))
            {
                IndicatorMID = TeamIndicatorMesh->CreateAndSetMaterialInstanceDynamic(0);
            }

            if (IndicatorMID)
            {
                FLinearColor TargetColor = FLinearColor::White;
                if (PS->GetTeam() == Teams::Red) TargetColor = FLinearColor::Red;
                else if (PS->GetTeam() == Teams::Blue) TargetColor = FLinearColor::Blue;

                // "Color" 파라미터로 색상 전달 (머티리얼에 이 파라미터가 있어야 함)
                IndicatorMID->SetVectorParameterValue(TEXT("Color"), TargetColor);
            }
        }
    }
}

// Called to bind functionality to input
void ATTLobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATTLobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTLobbyCharacter, HeadMesh);
	DOREPLIFETIME(ATTLobbyCharacter, BodyMesh);
	DOREPLIFETIME(ATTLobbyCharacter, CurrentHeadIndex);
	DOREPLIFETIME(ATTLobbyCharacter, CurrentBodyIndex);
}

#pragma endregion

#pragma region Replication Callbacks

void ATTLobbyCharacter::OnRep_HeadMesh()
{
	if (Head)
	{
		Head->SetSkeletalMesh(HeadMesh);
	}
}

void ATTLobbyCharacter::OnRep_BodyMesh()
{
	if (GetMesh())
	{
		GetMesh()->SetSkeletalMesh(BodyMesh);
	}
}

#pragma endregion

#pragma region Server RPCs

void ATTLobbyCharacter::ServerChangeHeadMeshByIndex_Implementation(int32 MeshIndex)
{
	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect>();
	if (CDO && CDO->PlayerCharacterHeadSkeletalPaths.IsValidIndex(MeshIndex))
	{
		CurrentHeadIndex = MeshIndex;
		FSoftObjectPath MeshPath = CDO->PlayerCharacterHeadSkeletalPaths[MeshIndex];
		
		// 서버 동기 로드
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			HeadMesh = LoadedMesh;
			OnRep_HeadMesh(); // 서버에서 적용
			
			// 메인 레벨로의 Seamless Travel을 위해 PlayerState에 지속
			if (AController* Ctrl = GetController())
			{
				if (ATTPlayerState* PS = Ctrl->GetPlayerState<ATTPlayerState>())
				{
					PS->PersistedHeadMesh = LoadedMesh;
				}
			}
		}
	}
}

bool ATTLobbyCharacter::ServerChangeHeadMeshByIndex_Validate(int32 MeshIndex)
{
	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect>();
	return CDO && CDO->PlayerCharacterHeadSkeletalPaths.IsValidIndex(MeshIndex);
}

void ATTLobbyCharacter::ServerChangeBodyMeshByIndex_Implementation(int32 MeshIndex)
{
	const UTTCharactorSkeletalMeshSelect* CDO = GetDefault<UTTCharactorSkeletalMeshSelect>();
	
	if (CDO && CDO->PlayerCharacterSkeletalPaths.IsValidIndex(MeshIndex))
	{
		CurrentBodyIndex = MeshIndex;
		FSoftObjectPath MeshPath = CDO->PlayerCharacterSkeletalPaths[MeshIndex];
		
		// 서버 동기 로드
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			BodyMesh = LoadedMesh;
			OnRep_BodyMesh(); // 서버에서 적용
			
			// 메인 레벨로의 Seamless Travel을 위해 PlayerState에 지속
			if (AController* Ctrl = GetController())
			{
				if (ATTPlayerState* PS = Ctrl->GetPlayerState<ATTPlayerState>())
				{
					PS->PersistedBodyMesh = LoadedMesh;
				}
			}
		}
	}
}

bool ATTLobbyCharacter::ServerChangeBodyMeshByIndex_Validate(int32 MeshIndex)
{
	const UTTCharactorSkeletalMeshSelect* CDO = GetDefault<UTTCharactorSkeletalMeshSelect>();
	return CDO && CDO->PlayerCharacterSkeletalPaths.IsValidIndex(MeshIndex);
}

#pragma endregion

#pragma region Helpers

void ATTLobbyCharacter::ChangeHead(int32 Index)
{
	// 로컬 GameInstance에 저장 (클라이언트)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance()))
		{
			GI->CustomizedHeadIndex = Index;
		}
	}

	if (HasAuthority())
	{
		// 서버 - 직접 Implementation 호출
		ServerChangeHeadMeshByIndex_Implementation(Index);
	}
	else
	{
		// 클라이언트 - RPC 호출
		ServerChangeHeadMeshByIndex(Index);
	}
}

void ATTLobbyCharacter::ChangeBody(int32 Index)
{
	// 로컬 GameInstance에 저장 (클라이언트)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance()))
		{
			GI->CustomizedBodyIndex = Index;
		}
	}

	if (HasAuthority())
	{
		// 서버 - 직접 Implementation 호출
		ServerChangeBodyMeshByIndex_Implementation(Index);
	}
	else
	{
		// 클라이언트 - RPC 호출
		ServerChangeBodyMeshByIndex(Index);
	}
}

#pragma endregion

