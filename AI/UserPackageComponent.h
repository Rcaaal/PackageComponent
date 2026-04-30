// UserPackageComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/ItemDataAsset.h"
#include "UserPackageComponent.generated.h"

class UPackagePanelWidget;
class APlayerController;
class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUserPackageComponentChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserPackageItemAdded, FName, ItemID, int32, AddCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserPackageItemRemoved, FName, ItemID, int32, RemoveCount);

UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class PACKAGECOMPONENT_API UUserPackageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUserPackageComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** 背包格子数量 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage", meta=(ClampMin="1", UIMin="1"))
	int32 PackageSlotNum = 30;

	/** 物品数据库（你现有逻辑） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|Data")
	TArray<TObjectPtr<UItemDataAsset>> ItemDataBase;

	/** 背包槽位数组 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UserPackage")
	TArray<FInventorySlot> Slots;

	/** 背包变化回调 */
	UPROPERTY(BlueprintAssignable, Category="UserPackage|Event")
	FOnUserPackageComponentChanged OnUserPackageComponentChanged;

	/** 新增道具回调 */
	UPROPERTY(BlueprintAssignable, Category="UserPackage|Event")
	FOnUserPackageItemAdded OnUserPackageItemAdded;

	/** 移除道具回调 */
	UPROPERTY(BlueprintAssignable, Category="UserPackage|Event")
	FOnUserPackageItemRemoved OnUserPackageItemRemoved;

	/** 是否启用组件内UI管理（挂在NPC可关掉） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|UI")
	bool bEnablePackageUI = true;

	/** 背包面板类（WBP_Package） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|UI")
	TSubclassOf<UPackagePanelWidget> PackageWidgetClass;

	/** 背包面板默认可见性 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UserPackage|UI")
	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;

	/** 初始化背包 */
	UFUNCTION(BlueprintCallable, Category="UserPackage")
	void InitPackage(int InitSlotNums = -1);

	/** 新增道具 */
	UFUNCTION(BlueprintCallable, Category="UserPackage")
	int32 AddItem(FName ItemID, int32 AddCount);

	/** 移除道具 */
	UFUNCTION(BlueprintCallable, Category="UserPackage")
	int32 RemoveItem(FName ItemID, int32 RemoveCount);

	/** 查询道具总数 */
	UFUNCTION(BlueprintPure, Category="UserPackage")
	int32 GetItemTotalCount(FName ItemID) const;

	/** 是否可消耗 */
	UFUNCTION(BlueprintPure, Category="UserPackage")
	bool CanConsumeItem(FName ItemID, int32 ConsumeCount) const;

	/** 是否可添加 */
	UFUNCTION(BlueprintPure, Category="UserPackage")
	bool CanAddItem(FName ItemID, int32 AddCount) const;

	/** 空槽数量 */
	UFUNCTION(BlueprintPure, Category="UserPackage")
	int32 GetEmptySlotCount() const;

	/** 获取槽位数组（只读） */
	UFUNCTION(BlueprintPure, Category="UserPackage")
	const TArray<FInventorySlot>& GetSlots() const;

	/** 切换背包UI显示（供PlayerController Tab调用） */
	UFUNCTION(BlueprintCallable, Category="UserPackage|UI")
	void TogglePackageUI();

	/** 显示背包UI */
	UFUNCTION(BlueprintCallable, Category="UserPackage|UI")
	void ShowPackageUI();

	/** 隐藏背包UI */
	UFUNCTION(BlueprintCallable, Category="UserPackage|UI")
	void HidePackageUI();

	/** 当前是否显示 */
	UFUNCTION(BlueprintPure, Category="UserPackage|UI")
	bool IsPackageUIVisible() const;

	/** 获取已创建的UI实例 */
	UFUNCTION(BlueprintPure, Category="UserPackage|UI")
	UPackagePanelWidget* GetPackageWidget() const { return PackageWidgetInstance; }

private:
	/** 当前UI是否显示 */
	bool bPackageUIVisible = false;

	/** 背包Widget实例 */
	UPROPERTY(Transient)
	TObjectPtr<UPackagePanelWidget> PackageWidgetInstance = nullptr;

	/** 最大堆叠查询 */
	int32 GetMaxStackCount(FName ItemID) const;

	/** 清空槽位 */
	void ClearSlot(FInventorySlot& Slot);

	/** 写入槽位 */
	void SetSlotItem(FInventorySlot& Slot, FName ItemID, int32 Count);

	/** 创建并初始化Widget */
	void CreatePackageWidgetIfNeeded();

	/** 查找本地玩家控制器 */
	APlayerController* ResolveLocalPlayerController() const;
};
