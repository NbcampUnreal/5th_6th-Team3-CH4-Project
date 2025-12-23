// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserNameBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAM03_API UUserNameBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct () override;

	void SetName ( const FName& UserName );
protected:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<class UTextBlock> PlayerName;
};
