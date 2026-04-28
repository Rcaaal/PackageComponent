// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PackageComponentCharacter.h"
#include "Package/PickupActor/PickupActorBase.h"
#include "BaseCanPickCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PACKAGECOMPONENT_API ABaseCanPickCharacter : public APackageComponentCharacter
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetCurrentPick(APickupActorBase* InPickup);
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void ClearCurrentPick(APickupActorBase* InPickup);
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void TryPickupNow();
	
protected:
	virtual void BeginPlay() override;
	
	//拾取UI
	UPROPERTY(EditDefaultsOnly,Category = "UI")
	TSubclassOf<UUserWidget> InteractWidgetClass;
	
private:
	UPROPERTY()
	TObjectPtr<APickupActorBase> CurrentPick = nullptr;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> InteractWidget = nullptr;
	
	void ShowInteractWidget(bool bShow);
};
