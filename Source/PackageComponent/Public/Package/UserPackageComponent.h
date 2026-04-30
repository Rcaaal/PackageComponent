// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "DataAsset/ItemDataAsset.h"
#include "UserPackageComponent.generated.h"

class UPackageUserWidget;
//用于参数表格
USTRUCT(BlueprintType)
struct FUserPackageItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|DataTable")
	TSoftObjectPtr<UItemDataAsset> ItemData;
};

//当背包状态改变时回调，用于背包刷新
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUserPackageComponentChanged);
//新增时回调
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserPackageItemAdded,FName,ItemID,int32,AddCount);
//删除时回调
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserPackageItemRemoved,FName,ItemID,int32,RemoveCount);


UCLASS( ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent) )
class PACKAGECOMPONENT_API UUserPackageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUserPackageComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	//背包格数
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "UserPackage",meta=(ClampMin = "1",UIMin = "1"))
	int32 PackageSlotNum = 30;

	//拾取道具的信息 手动补充数据库
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "UserPackage | Data")
	TArray<TObjectPtr<UItemDataAsset>> ItemDataBase;
	
	// DataTable 数据源（推荐）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Package|DataTable")
	TObjectPtr<UDataTable> ItemDataTable = nullptr;

	// BeginPlay 自动从 DataTable 构建 ItemDataBase
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|DataTable")
	bool bAutoLoadItemDatabaseFromDataTable = true;

	// true: 追加到 ItemDataBase；false: 覆盖 ItemDataBase
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|DataTable")
	bool bAppendWhenAutoLoad = false;

	// true: 同步加载软引用（推荐 true）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|DataTable")
	bool bSyncLoadSoftRef = true;
	
	//背包插槽 结构体定义来自UItemDataAsset
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "UserPackage")
	TArray<FInventorySlot> Slots;
	
	//背包事件 回调
	UPROPERTY(BlueprintAssignable,Category = "UserPackage | Event")
	FOnUserPackageComponentChanged OnUserPackageComponentChanged;
	
	//背包新增 回调
	UPROPERTY(BlueprintAssignable,Category = "UserPackage | Event")
	FOnUserPackageItemAdded OnUserPackageItemAdded;
	
	//背包删减 回调
	UPROPERTY(BlueprintAssignable,Category = "UserPackage | Event")
	FOnUserPackageItemRemoved OnUserPackageItemRemoved;
	
	//背包初始化 参数大于0则使用传入参数 否则使用默认值
	UFUNCTION(BlueprintCallable,Category= "UserPackage")
	void InitPackage(int InitSlotNums = -1);
	
	///新增道具
	UFUNCTION(BlueprintCallable,Category= "UserPackage")
	int32 AddItem(FName ItemID,int32 AddCount);
	
	//移除道具
	UFUNCTION(BlueprintCallable,Category= "UserPackage")
	int32 RemoveItem(FName ItemID,int32 RemoveCount);
	
	//获取某个Item在背包中的数量总合
	UFUNCTION(BlueprintPure,Category= "UserPackage")
	int32 GetItemTotalCount(FName ItemID) const;
	
	//获取当前是否满足条件消耗某个Item（Item使用时）
	UFUNCTION(BlueprintPure,Category= "UserPackage")
	bool CanConsumeItem(FName ItemID,int32 ConsumeCount) const;
	
	//获取当前是否可以加入某个Item（Item加入时）
	UFUNCTION(BlueprintPure,Category= "UserPackage")
	bool CanAddItem(FName ItemID,int32 AddCount) const;
	
	//获取空余可用Slot数量
	UFUNCTION(BlueprintPure,Category= "UserPackage")
	int32 GetEmptySlotCount() const;
	
	//返回背包引用 用于刷新UI/调试/只读判断
	UFUNCTION(BlueprintPure,Category= "UserPackage")
	const TArray<FInventorySlot>& GetSlots() const;
	
	// 手动重建数据库（Blueprint 可调用）
	UFUNCTION(BlueprintCallable, Category="Package|DataTable")
	void RebuildItemDatabaseFromDataTable(bool bAppend = false, bool bLoadSync = true);
private:
	//获取某Item的最大堆叠数 通过DataAsset获取 用于AddItem/CanAddItem
	int32 GetMaxStackCount(FName ItemID) const;
	
	//清理某Slot 通常在RemoveItem后堆叠为0的时候调用
	void ClearSlot(FInventorySlot& Slot);
	
	//向某个格子中写入信息
	void SetSlotItem(FInventorySlot& Slot,FName ItemID,int32 Count);

	//用于数据读取
	void RebuildItemLookup();
	void AddItemDataUnique(UItemDataAsset* InItemData);
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UItemDataAsset>> ItemLookupMap;

public:
	//UI
	//是否启用PackageUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|UI")
	bool bEnablePackageUI = true;

	//背包UI类
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|UI")
	TSubclassOf<UPackageUserWidget> PackageWidgetClass;

	//UI可见性
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|UI")
	ESlateVisibility PackageVisibility = ESlateVisibility::Collapsed;

	//背包显示状态切换
	UFUNCTION(BlueprintCallable,Category="UserPackage|UI")
	void TogglePackageUI();

	//显示背包
	UFUNCTION(BlueprintCallable,Category="UserPackage|UI")
	void ShowPackageUI();

	//隐藏背包
	UFUNCTION(BlueprintCallable,Category="UserPackage|UI")
	void HidePackageUI();

	//获取当前可见性
	UFUNCTION(BlueprintCallable,Category="UserPackage|UI")
	bool IsPackageUIVisible() const;

	//获取UI实例
	UFUNCTION(BlueprintCallable,Category="UserPackage|UI")
	UPackageUserWidget* GetPackageUIWidget() const { return PackageWidgetInstance; };

private:
	//UI实例
	//不参与序列化 不参与网络复制
	UPROPERTY(Transient)
	TObjectPtr<UPackageUserWidget> PackageWidgetInstance = nullptr;

	//当前Package是否显示
	bool bPackageUIVisible = false;

	void CreatePackageUI();

	APlayerController* GetLocalPlayerController();
};
