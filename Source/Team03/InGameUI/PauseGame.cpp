// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseGame.h"
#include "Components/Button.h"
#include "../Controller/TTPlayerController.h"
#include "../Outgame/UW_TitleLevel.h"
#include "Kismet/GameplayStatics.h"
#include "Outgame/UW_Option.h"
#include "Animation/WidgetAnimation.h"
//#include "Kismet/KismetSystemLibrary.h"

void UPauseGame::NativeConstruct ()
{
	Super::NativeConstruct ();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic ( this , &ThisClass::OnContinueButtonClicked );
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic ( this , &ThisClass::OnExitButtonClicked );
	}
	if (OptionWidgetClass)
	{
		OptionWidget = CreateWidget<UUW_Option> (GetWorld(), OptionWidgetClass );
	}
	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic ( this , &ThisClass::OnOptionButtonClicked );
	}

	if (Anim_SlideIn)
	{
		PlayAnimation ( Anim_SlideIn );
	}

}

void UPauseGame::OnContinueButtonClicked ()
{
	// TODO : UI 없애기	, 게임 재개
	RemoveFromParent ();
	if (ATTPlayerController* PC = Cast<ATTPlayerController> ( GetOwningPlayer () ))
	{
		PC->SetShowMouseCursor ( false );
		FInputModeGameOnly InputMode;
		PC->SetInputMode ( InputMode );
	}
}

void UPauseGame::OnExitButtonClicked ()
{
	// TODO : 메인화면으로 이동, 일단 강제 종료
	//UKismetSystemLibrary::QuitGame ( this , nullptr , EQuitPreference::Quit , false );
	RemoveFromParent ();
	UGameplayStatics::OpenLevel ( this , FName ( TEXT ( "TitleLevel" ) ) );
}

void UPauseGame::OnOptionButtonClicked ()
{
	OptionWidget->AddToViewport ();
}
