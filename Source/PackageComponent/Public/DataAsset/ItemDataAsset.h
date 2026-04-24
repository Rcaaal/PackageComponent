// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */
class UTexture2D;

//枚举类型 用来标记当前物品属性
UENUM(BlueprintType)
enum EItemType : uint8
{
	Consumable UMETA(DisplayName = "Consumable"),	//消耗品
	Material UMETA(DisplayName = "Material"),		//材料
	Quest UMETA(DisplayName = "Quest"),				//道具
	Equipment UMETA(DisplayName = "Equipment"),		//装备
	Misc UMETA(DisplayName = "Misc"),				//其他
};

UCLASS()
class PACKAGECOMPONENT_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
};
