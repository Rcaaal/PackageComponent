// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.generated.h"

/**
 * 
 */

class UCountComponent;
class UOverlay;
class UCanvasPanel;
class USizeBox;
class UTextBlock;
class UVerticalBox;

UCLASS(BlueprintType, Blueprintable)
class PACKAGECOMPONENT_API UBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvas = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> InfoLayer = nullptr;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtCount = nullptr;
	
	UFUNCTION()
	void HandleCountTick(float RemainingSeconds);
	
	UFUNCTION()
	void HandleCountFinished();
	
private:
	UPROPERTY()
	TObjectPtr<UCountComponent> CachedCountComponent = nullptr;
};
