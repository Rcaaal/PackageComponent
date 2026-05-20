// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PackageComponentGameMode.generated.h"

UCLASS(minimalapi)
class APackageComponentGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APackageComponentGameMode();
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Count")
	void TriggerStartCount(float InDurationSeconds = -1.f);
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Count")
	float StartCountDuration = 60.f;
	
private:
	UFUNCTION()
	void HandleCountTick(float RemainingSeconds);
	
	UFUNCTION()
	void HandleCountFinished();
};



