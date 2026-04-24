// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActorBase.generated.h"


class UItemDataAsset;
class UStaticMeshComponent;
class USphereComponent;
class USceneComponent;

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

//拾取成功代理 广播拾取的Actor以及数量
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupSucceeded,AActor*,Interactor,int32,AcceptedCount);
//拾取失败代理 广播尝试拾取的Actor以及失败原因
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupFailed,AActor*,Interactor,EPickupFailReason,Reason);

UCLASS()
class PACKAGECOMPONENT_API APickupActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActorBase();
	
	//是否可交互
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool CanInteract(AActor* Interactor) const;
	
	//尝试拾取
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool TryPickup(AActor* Interactor);
	
	//获取交互文本
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FText GetInteractorText() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//根组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;
	
	//外观
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	
	//碰撞检测
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractorComponent;

	//数据资产
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Data")
	UItemDataAsset* ItemData = nullptr;
	
	//单次拾取数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Data", meta = (ClampMin = "1", UIMin = "1"))
	int32 Count = 1;
	
	//拾取后是否销毁
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Behavior")
	bool bDestroyOnPickupSuccess = true;
	
	//可拾取距离
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Behavior", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxInteractorDis = 100.0f;
	
	//拾取成功回调
	UPROPERTY(BlueprintAssignable, Category = "Pickup|Event")
	FOnPickupSucceeded PickupSucceeded;
	
	//拾取失败回调
	UPROPERTY(BlueprintAssignable, Category = "Pickup|Event")
	FOnPickupFailed PickupFailed;
	
	//蓝图实现 用于确认是成功将拾取物加入拾取者的背包 将拾取物的ItemID和拾取数量添加到拾取者的背包中 并返回拾取成功与否
	//Interactor拾取者实例 ItemID拾取物ID
	//InCount本次请求添加的拾取物数量 OnAcceptCount本次实际添加成功的拾取物数量（由当前拾取者背包实际情况决定）
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup|K2", meta = (DisplayName = "Request Add Item To Interactor"))
	bool K2_RequestAddItemToInteractor(AActor* Interactor,FName ItemID,int32 InCount,int32& OnAcceptedCount);
	
	//拾取成功回调
	//蓝图实现函数后优先调用蓝图，否则调用_Implementation
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickupSuccess(AActor* Interactor,int32 AcceptedCount);
	virtual void OnPickupSuccess_Implementation(AActor* Interactor,int32 AcceptedCount);
	
	//拾取失败回调
	//蓝图实现函数后优先调用蓝图，否则调用_Implementation
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickupFailed(AActor* Interactor,EPickupFailReason Reason);
	virtual void OnPickupFailed_Implementation(AActor* Interactor,EPickupFailReason Reason);
	
private:
	//拾取校验
	bool ValidatePickup(AActor* Interactor,EPickupFailReason& FailReason) const;
	//拾取者位置校验
	bool IsInteractorInRange(AActor* Interactor) const;
};
