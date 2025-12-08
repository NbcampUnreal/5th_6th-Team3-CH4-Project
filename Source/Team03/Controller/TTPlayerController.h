// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class TEAM03_API ATTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATTPlayerController ();


	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputMappingContext* InputMappingContext;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* MoveAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* JumpAction;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Input" )
	UInputAction* SprintAction;

	virtual void BeginPlay () override;
	
};
