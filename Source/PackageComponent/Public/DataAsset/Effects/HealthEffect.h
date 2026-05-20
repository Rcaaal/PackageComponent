// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/ItemEffectBase.h"
#include "HealthEffect.generated.h"

/**
 * 
 */
UCLASS()
class PACKAGECOMPONENT_API UHealthEffect : public UItemEffectBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="HealthEffect")
	float HealthValue = 50.f;
	
	virtual bool CanApply_Implementation(AActor* TargetActor) const override;
	virtual void ApplyEffect_Implementation(AActor* TargetActor) override;
};
