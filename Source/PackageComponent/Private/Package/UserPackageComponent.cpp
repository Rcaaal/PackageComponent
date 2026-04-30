// Fill out your copyright notice in the Description page of Project Settings.


#include "Package/UserPackageComponent.h"

#include "UMG/PackageUserWidget.h"

// Sets default values for this component's properties
UUserPackageComponent::UUserPackageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUserPackageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//初始化Package
	if (Slots.Num() != PackageSlotNum)
	{
		InitPackage(PackageSlotNum);
	}
	
	//读取ItemTable
	if (bAutoLoadItemDatabaseFromDataTable)
	{
		RebuildItemDatabaseFromDataTable(bAppendWhenAutoLoad, bSyncLoadSoftRef);
	}
	else
	{
		RebuildItemLookup();
	}
}

void UUserPackageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UUserPackageComponent::InitPackage(int InitSlotNums)
{
	if (InitSlotNums > 0)
	{
		PackageSlotNum = InitSlotNums;
	}
	
	PackageSlotNum = FMath::Max(PackageSlotNum, 1);
	Slots.SetNum(PackageSlotNum);
	for (int i = 0; i < Slots.Num(); ++i)
	{
		Slots[i].SlotIndex = i;
		ClearSlot(Slots[i]);
	}
	
	OnUserPackageComponentChanged.Broadcast();
}

int32 UUserPackageComponent::AddItem(FName ItemID, int32 AddCount)
{
	if (ItemID.IsNone() || AddCount <= 0 || Slots.Num() <= 0)
	{
		return 0;
	}
	
	const int32 CurrentItemMaxStackCount = FMath::Max(1, GetMaxStackCount(ItemID));
	int32 AddRemainingCount = AddCount;
	
	//分支一 优先加入已有堆叠
	for (FInventorySlot& Slot : Slots)
	{
		if (AddRemainingCount <= 0)
		{
			break;
		}
		//只针对已经占用的格子做处理
		//当前遍历格已占用才处理
		if (!Slot.bOccupied)
		{
			continue;
		}
		//筛选对应ItemID的已占用格
		if (Slot.ItemInstance.ItemID != ItemID)
		{
			continue;
		}
		//计算余下空间
		const int32 RemainingSpace = CurrentItemMaxStackCount - Slot.ItemInstance.Count;
		if (RemainingSpace <= 0)
		{
			continue;
		}
		//计算新加数量
		const int32 CurrentAddCount = FMath::Min(RemainingSpace, AddRemainingCount);
		Slot.ItemInstance.Count += CurrentAddCount;
		AddRemainingCount -= CurrentAddCount;
	}
	
	//分支二 占用空插槽（当前添加Item占满了上一个未占满格子且仍有余量/当前添加Item无已占格/当前添加Item所占格全满）
	for (FInventorySlot& Slot : Slots)
	{
		//剩余需添加数
		if (AddRemainingCount <= 0)
		{
			break;
		}
		//当前遍历格未占用才处理
		if (Slot.bOccupied)
		{
			continue;
		}
		const int32 CurrentAddCount = FMath::Min(CurrentItemMaxStackCount,AddRemainingCount);
		SetSlotItem(Slot,ItemID,CurrentAddCount);
		AddRemainingCount -= CurrentAddCount;
	}
	//计算此次添加成功的数量
	const int32 AddedCount = AddCount - AddRemainingCount;
	if (AddedCount > 0)
	{
		OnUserPackageComponentChanged.Broadcast();
		OnUserPackageItemAdded.Broadcast(ItemID,AddedCount);
	}
	return AddedCount;
}

int32 UUserPackageComponent::RemoveItem(FName ItemID, int32 RemoveCount)
{
	if (ItemID.IsNone() || RemoveCount <= 0 || Slots.Num() <= 0)
	{
		return 0;
	}
	
	int32 RemoveRemainingCount = RemoveCount;
	for (FInventorySlot& Slot : Slots)
	{
		if (RemoveRemainingCount <= 0)
		{
			break;
		}
		//当前格未占用
		if (!Slot.bOccupied)
		{
			continue;
		}
		//当前ItemID与移除ID不对应
		if (Slot.ItemInstance.ItemID != ItemID)
		{
			continue;
		}
		//此次可移除最大值
		const int32 CurrentRemoveCount = FMath::Min(Slot.ItemInstance.Count,RemoveRemainingCount);
		//更新当前格移除后剩余堆叠数
		Slot.ItemInstance.Count -= CurrentRemoveCount;
		//更新本次移除后仍需移除数量
		RemoveRemainingCount -= CurrentRemoveCount;

		//移除后 插槽中余下Item数归0 则清空插槽 重置插槽属性
		if (Slot.ItemInstance.Count <= 0)
		{
			ClearSlot(Slot);
		}
	}
		//计算此次清除Item数量
		const int32 RemovedCount = RemoveCount - RemoveRemainingCount;
		if (RemovedCount > 0)
		{
			OnUserPackageComponentChanged.Broadcast();
			OnUserPackageItemRemoved.Broadcast(ItemID,RemovedCount);
		}
		return RemovedCount;
}

//查询某Item总量（跨格查询）
int32 UUserPackageComponent::GetItemTotalCount(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return 0;
	}
	int32 TotalCount = 0;
	for (const FInventorySlot& Slot : Slots)
	{
		if (Slot.bOccupied && Slot.ItemInstance.ItemID == ItemID)
		{
			TotalCount += Slot.ItemInstance.Count;
		}
	}
	
	return TotalCount;
}

//查询是否可以消耗
bool UUserPackageComponent::CanConsumeItem(FName ItemID, int32 ConsumeCount) const
{
	if (ConsumeCount <= 0)
	{
		return true;
	}
	return GetItemTotalCount(ItemID) >= ConsumeCount;
}

//查询是否可以新增
bool UUserPackageComponent::CanAddItem(FName ItemID, int32 AddCount) const
{
	if (ItemID.IsNone() || AddCount <= 0)
	{
		return false;
	}
	//获取当前Item的《单格》堆叠上限
	const int32 CurrentItemMaxStackCount = FMath::Max(1, GetMaxStackCount(ItemID));
	//初始化当前Item可堆叠总数
	int32 RemainingCount = 0;
	//遍历所有插槽
	for (const FInventorySlot& Slot : Slots)
	{
		//插槽为空
		if (!Slot.bOccupied)
		{
			//当前插槽可全用于存放该Item
			RemainingCount += CurrentItemMaxStackCount;
			continue;
		}
		//插槽非空且存放当前Item
		if (Slot.ItemInstance.ItemID == ItemID)
		{
			//可用：当前Item堆叠上限减去已堆叠Item数
			RemainingCount += FMath::Max(0,CurrentItemMaxStackCount - Slot.ItemInstance.Count);
		}
		//插槽非空但不存放当前Item直接忽略
	}
	return RemainingCount >= AddCount;
}

int32 UUserPackageComponent::GetEmptySlotCount() const
{
	int EmptySlotCount = 0;
	for (const FInventorySlot& Slot : Slots)
	{
		if (!Slot.bOccupied)
		{
			++EmptySlotCount;
		}
	}
	return EmptySlotCount;
}

const TArray<FInventorySlot>& UUserPackageComponent::GetSlots() const
{
	return Slots;
}

void UUserPackageComponent::RebuildItemDatabaseFromDataTable(bool bAppend, bool bLoadSync)
{
	if (!bAppend)
	{
		ItemDataBase.Reset();
	}
	if (!ItemDataTable)
	{
		RebuildItemLookup();
		return;
	}
	const TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FUserPackageItemRow* Row = ItemDataTable->FindRow<FUserPackageItemRow>(RowName,TEXT("RebuildItemDatabaseFromDataTable"));

		if (!Row)
		{
			continue;
		}
		
		UItemDataAsset* LoadedData = bLoadSync ? Row->ItemData.LoadSynchronous() : Row->ItemData.Get();
		AddItemDataUnique(LoadedData);
	}
	RebuildItemLookup();
}

int32 UUserPackageComponent::GetMaxStackCount(FName ItemID) const
{
	//从DataAsset获取ItemID对应的MaxStack（常量）
	for (const UItemDataAsset* Data : ItemDataBase)
	{
		if (!Data)
		{
			continue;
		}
		if (Data->ItemID == ItemID)
		{
			return FMath::Max(1,Data->MaxStack);
		}
	}
	return 1;
}

void UUserPackageComponent::ClearSlot(FInventorySlot& Slot)
{
	Slot.bOccupied = false;
	Slot.ItemInstance.ItemID = NAME_None;
	Slot.ItemInstance.Count = 0;
}

void UUserPackageComponent::SetSlotItem(FInventorySlot& Slot, FName ItemID, int32 Count)
{
	Slot.bOccupied = true;
	Slot.ItemInstance.ItemID = ItemID;
	Slot.ItemInstance.Count = FMath::Max(1,Count);
}

void UUserPackageComponent::RebuildItemLookup()
{
	ItemLookupMap.Reset();
	
	for (UItemDataAsset* Data : ItemDataBase)
	{
		if (!Data || Data->ItemID.IsNone())
		{
			continue;
		}
		ItemLookupMap.FindOrAdd(Data->ItemID) = Data;
	}
}

void UUserPackageComponent::AddItemDataUnique(UItemDataAsset* InItemData)
{
	if (!InItemData || InItemData->ItemID.IsNone())
	{
		return;
	}
	for (const UItemDataAsset* DataAsset : ItemDataBase)
	{
		if (DataAsset == InItemData)
		{
			return;
		}
	}
	ItemDataBase.Add(InItemData);
}

void UUserPackageComponent::TogglePackageUI()
{
	if (!bPackageUIVisible)
	{
		ShowPackageUI();
	}
	else
	{
		HidePackageUI();
	}
}

void UUserPackageComponent::ShowPackageUI()
{
	CreatePackageUI();
	if (!PackageWidgetInstance)
	{
		return;
	}

	bPackageUIVisible = true;
	PackageWidgetInstance->SetVisibility(ESlateVisibility::Visible);

	if (APlayerController* PC = GetLocalPlayerController())
	{
		FInputModeGameAndUI Mode;
		Mode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = true;
	}
}

void UUserPackageComponent::HidePackageUI()
{
	if (!PackageWidgetInstance)
	{
		return;
	}

	bPackageUIVisible = false;
	PackageWidgetInstance->SetVisibility(PackageVisibility);

	if (APlayerController* PC = GetLocalPlayerController())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

bool UUserPackageComponent::IsPackageUIVisible() const
{
	return bPackageUIVisible;
}

void UUserPackageComponent::CreatePackageUI()
{
	if (!bEnablePackageUI || !PackageWidgetClass || PackageWidgetInstance)
	{
		return;
	}
	APlayerController* PC = GetLocalPlayerController();
	if (!PC || !PC->IsLocalController())
	{
		return;
	}
	PackageWidgetInstance = CreateWidget<UPackageUserWidget>(PC,PackageWidgetClass);
	if (PackageWidgetInstance)
	{
		PackageWidgetInstance->AddToViewport();
		PackageWidgetInstance->SetVisibility(PackageVisibility);

		PackageWidgetInstance->InitWithPackageComponent(this);
		bPackageUIVisible = false;
	}
	else
	{
		return;
	}
	
}

APlayerController* UUserPackageComponent::GetLocalPlayerController()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		return Cast<APlayerController>(OwnerPawn->GetController());
	}

	return Cast<APlayerController>(OwnerActor->GetInstigatorController());
}


