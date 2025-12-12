// Fill out your copyright notice in the Description page of Project Settings.


#include "TTAnimInstance.h"


void UTTAnimInstance::AnimNotify_CheckHit ()
{
	if (OnCheckHit.IsBound () == true)
	{
		OnCheckHit.Broadcast ();
	}
}
