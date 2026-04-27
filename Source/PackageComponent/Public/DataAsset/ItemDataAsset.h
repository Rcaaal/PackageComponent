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
enum class EItemType : uint8
{
	Consumable UMETA(DisplayName = "Consumable"),	//消耗品
	Material UMETA(DisplayName = "Material"),		//材料
	Quest UMETA(DisplayName = "Quest"),				//道具
	Equipment UMETA(DisplayName = "Equipment"),		//装备
	Misc UMETA(DisplayName = "Misc"),				//其他
};

//结构体 用于道具实例的可扩展动态数据 主要存储动态内容
USTRUCT(BlueprintType)
struct PACKAGECOMPONENT_API FItemCustomData
{
	GENERATED_BODY()
	
	//耐久度 -1即为不可使用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Durability = -1.0f;
	
	//通用数据字典
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TMap<FName, float> CustomData;
};

//结构体 用于存储道具实例的信息
USTRUCT(BlueprintType)
struct PACKAGECOMPONENT_API FItemInstance
{
	GENERATED_BODY()
	
	//ItemID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID = NAME_None;
	
	//Item数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Count = 1;
	
	//Item自身动态数据信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemCustomData CustomData;
	
	bool IsValid() const
	{
		return !ItemID.IsNone() && Count > 0 ;
	}
};

//结构体 用于表示Package中一个格子当前的状态信息
USTRUCT(BlueprintType)
struct PACKAGECOMPONENT_API FInventorySlot
{
	GENERATED_BODY()
	
	//当前格子所占位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SlotIndex = INDEX_NONE;
	
	//当前格子存放内容
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FItemInstance ItemInstance;
	
	//当前格子是否占用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bOccupied = false;
	
	//清空当前格子
	void Clear()
	{
		ItemInstance = FItemInstance();
		bOccupied = false;
	}
	
	//将Item加入格子
	void SetItem(const FItemInstance& InItem)
	{
		ItemInstance = InItem;
		bOccupied = InItem.IsValid();
	}
};

//以上为运行时数据结构
//以下为道具模板/配置数据

UCLASS(BlueprintType)
class PACKAGECOMPONENT_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	//ID主键 唯一标识 与ItemInstance中的ItemID一一对应
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemID = NAME_None;
	
	//展示名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;
	
	//展示图片
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;
	
	//道具最大可堆叠数
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item",meta=(ClampMin = "1", UIMin = "1"))
	int32 MaxStack = 1;
	
	//默认道具类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemType ItemType = EItemType::Misc;
	
	//道具描述
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (MultiLine = "true"))
	FText Description;
	
	//给AssetManager用 用于标明此Asset的名称和归类
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
