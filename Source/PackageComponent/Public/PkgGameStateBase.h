// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PkgGameStateBase.generated.h"

/**
 * 
 */
class UCountComponent;

UCLASS()
class PACKAGECOMPONENT_API APkgGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	APkgGameStateBase();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Countdown")
	TObjectPtr<UCountComponent> CountComponent;
};
