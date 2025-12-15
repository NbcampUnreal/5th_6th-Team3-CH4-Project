// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TTLobbyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SelectSkeletal/TTCharactorHeadSkeletalSelect.h"
#include "SelectSkeletal/TTCharactorSkeletalMeshSelect.h"
#include "Character/TTPlayerState.h"
#include "Outgame/TTGameInstance.h"

// Sets default values
ATTLobbyCharacter::ATTLobbyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use the default Mesh as Body, or create a separate one.
	// For now, let's assume 'Mesh' inherited from ACharacter is the Body.
	// But if we want separate parts similar to TTPlayerCharacter:
	
	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(GetMesh());
	Head->SetIsReplicated(true);

	// Body usually refers to GetMesh(), so we might just use GetMesh() as the Body logic target,
	// or if 'Body' implies a specific part separate from the root mesh.
	// Let's assume GetMesh() is the main body for simplicity, 
	// but if user wants explicit separation:
	// Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body")); ...
	// However, ACharacter already has 'Mesh'. Let's alias logic to it or keep separate if needed.
	// Re-reading TTPlayerCharacter, it has 'Head' attached.

	// Initialize indices
	CurrentHeadIndex = 0;
	CurrentBodyIndex = 0;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ATTLobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATTLobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ATTLobbyCharacter::ServerChangeHeadMeshByIndex_Implementation(int32 MeshIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("[LOBBY-HEAD] ServerChangeHeadMeshByIndex: Index=%d"), MeshIndex);
	
	const UTTCharactorHeadSkeletalSelect* CDO = GetDefault<UTTCharactorHeadSkeletalSelect>();
	if (CDO && CDO->PlayerCharacterHeadSkeletalPaths.IsValidIndex(MeshIndex))
	{
		CurrentHeadIndex = MeshIndex;
		FSoftObjectPath MeshPath = CDO->PlayerCharacterHeadSkeletalPaths[MeshIndex];
		
		// Synchronous load for server (can be async if needed)
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("[LOBBY-HEAD] LoadedMesh Valid: %s"), *LoadedMesh->GetName());
			
			HeadMesh = LoadedMesh;
			OnRep_HeadMesh(); // Apply on server
			
			// Persist to PlayerState for Seamless Travel to Main Level
			if (AController* Ctrl = GetController())
			{
				UE_LOG(LogTemp, Warning, TEXT("[LOBBY-HEAD] Controller Valid"));
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
		
		// Synchronous load for server (can be async if needed)
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshPath.TryLoad());
		if (LoadedMesh)
		{
			BodyMesh = LoadedMesh;
			OnRep_BodyMesh(); // Apply on server
			
			// Persist to PlayerState for Seamless Travel to Main Level
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


void ATTLobbyCharacter::ChangeHead(int32 Index)
{
	// Save to Local GameInstance (Client)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance()))
		{
			GI->CustomizedHeadIndex = Index;
		}
	}

	if (HasAuthority())
	{
		// Server - 직접 Implementation 호출
		ServerChangeHeadMeshByIndex_Implementation(Index);
	}
	else
	{
		// Client - RPC 호출
		ServerChangeHeadMeshByIndex(Index);
	}
}

void ATTLobbyCharacter::ChangeBody(int32 Index)
{
	// Save to Local GameInstance (Client)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance()))
		{
			GI->CustomizedBodyIndex = Index;
		}
	}

	if (HasAuthority())
	{
		// Server - 직접 Implementation 호출
		ServerChangeBodyMeshByIndex_Implementation(Index);
	}
	else
	{
		// Client - RPC 호출
		ServerChangeBodyMeshByIndex(Index);
	}
}

