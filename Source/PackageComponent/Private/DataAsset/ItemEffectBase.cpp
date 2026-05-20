// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/ItemEffectBase.h"

bool UItemEffectBase::CanApply_Implementation(AActor* TargetActor) const
{
	return TargetActor != nullptr;
}

void UItemEffectBase::ApplyEffect_Implementation(AActor* TargetActor)
{
}
