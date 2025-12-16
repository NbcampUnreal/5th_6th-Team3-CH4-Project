// (c) 2024. Team03. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTTitlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API ATTTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATTTitlePlayerController();
	
	virtual void BeginPlay() override;
};
