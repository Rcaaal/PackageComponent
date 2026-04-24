// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActorBase.generated.h"


class UItemDataAsset;
class UStaticMeshComponent;

//枚举 用于标记拾取失败原因
UENUM(BlueprintType)
enum class EPickupFailReason : uint8
{
	None UMETA(DisplayName = "None"),							//成功
	InvalidItemData UMETA(DisplayName = "InvalidItemData"),		//Item数据无效
	InvalidCount UMETA(DisplayName = "InvilidCount"),			//Item数量无效
	OutOfRange UMETA(DisplayName = "OutOfRange"),				//超出拾取范围
	AddToInventoryFailed UMETA(DisplayName = "AddToInventoryFailed")	//加入背包失败（背包已满/组件不存在等）
};

UCLASS()
class PACKAGECOMPONENT_API APickupActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
