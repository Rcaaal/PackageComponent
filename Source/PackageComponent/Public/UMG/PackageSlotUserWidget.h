// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PackageSlotUserWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
/**
 * 
 */
UCLASS()
class PACKAGECOMPONENT_API UPackageSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//初始化格子显示内容
	UFUNCTION(BlueprintCallable,Category="PackageUI")
	void SetSlotData(UTexture2D* InIcon,int32 InCount);
	
protected:
	//图标
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_ItemIcon = nullptr;
	
	//数量
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count = nullptr;
};
