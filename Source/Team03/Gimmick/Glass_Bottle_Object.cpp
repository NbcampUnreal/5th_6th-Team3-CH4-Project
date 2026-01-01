// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Glass_Bottle_Object.h"

void AGlass_Bottle_Object::OnHit ( UPrimitiveComponent* HitComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , FVector NormalImpulse , const FHitResult& Hit )
{
	if (!HasAuthority ())
	{
		return;
	}
}
