// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/Effects/HealthEffect.h"

bool UHealthEffect::CanApply_Implementation(AActor* TargetActor) const
{
	return TargetActor != nullptr;
}

void UHealthEffect::ApplyEffect_Implementation(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}
	
	/*接入Health接口
	 * 
	 * 
	 * 
	 * 
	 * 
	 * 
	 * 
	 */
	
	UE_LOG(LogTemp, Log, TEXT("HealEffect Apply: Target=%s HealAmount=%.2f"),*GetNameSafe(TargetActor), HealthValue);
	Super::ApplyEffect_Implementation(TargetActor);
}
