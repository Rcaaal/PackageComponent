// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/PackageUserWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Package/UserPackageComponent.h"
#include "UMG/PackageSlotUserWidget.h"

void UPackageUserWidget::InitWithPackageComponent(UUserPackageComponent* InPackageComponent)
{
	if (!InPackageComponent)
	{
		return;
	}
	if (PackageComp)
	{
		PackageComp->OnUserPackageComponentChanged.RemoveDynamic(this,&UPackageUserWidget::OnPackageChanged);
		PackageComp->OnUserPackageItemAdded.RemoveDynamic(this,&UPackageUserWidget::OnItemAdded);
		PackageComp->OnUserPackageItemRemoved.RemoveDynamic(this,&UPackageUserWidget::OnItemRemoved);
	}
	PackageComp = InPackageComponent;
	PackageComp->OnUserPackageComponentChanged.AddDynamic(this,&UPackageUserWidget::OnPackageChanged);
	PackageComp->OnUserPackageItemAdded.AddDynamic(this,&UPackageUserWidget::OnItemAdded);
	PackageComp->OnUserPackageItemRemoved.AddDynamic(this,&UPackageUserWidget::OnItemRemoved);
	
	RefreshAllSlots();
}

void UPackageUserWidget::RefreshAllSlots()
{
	if (!Grid_Items || !PackageComp || !SlotWidgetClass)
	{
		return;
	}
	
	Grid_Items->ClearChildren();
	//读取背包数据
	const TArray<FInventorySlot>& Slots = PackageComp->GetSlots();
	//可见插槽的索引
	int32 VisibleSlotIndex = 0;
	
	for (const FInventorySlot& InventorySlot : Slots)
	{
		//排除无效值
		if (!InventorySlot.bOccupied || InventorySlot.ItemInstance.ItemID.IsNone() || InventorySlot.ItemInstance.Count <= 0)
		{
			continue;
		}
		//验证通过 填充数据
		UItemDataAsset* ItemData = FindItemDataByID(InventorySlot.ItemInstance.ItemID);
		UTexture2D* IconTexture = nullptr;
		if (ItemData)
		{
			IconTexture = ItemData->Icon.LoadSynchronous();
		}
		//创建图标Widget
		UPackageSlotUserWidget* SlotWidget = CreateWidget<UPackageSlotUserWidget>(this,SlotWidgetClass);

		if (!SlotWidget)
		{
			continue;
		}
		//填充数据
		SlotWidget->SetSlotData(IconTexture,InventorySlot.ItemInstance.Count);
		
		//计算网格行列坐标
		const int32 Row = VisibleSlotIndex / FMath::Max(1,ColumnCount);
		const int32 Column = VisibleSlotIndex % FMath::Max(1,ColumnCount);
		
		if (UUniformGridSlot* GridSlot = Grid_Items->AddChildToUniformGrid(SlotWidget,Row,Column))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
		
		++VisibleSlotIndex;
	}
}

void UPackageUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPackageUserWidget::NativeDestruct()
{
	if (PackageComp)
	{
		PackageComp->OnUserPackageComponentChanged.RemoveDynamic(this,&UPackageUserWidget::OnPackageChanged);
		PackageComp->OnUserPackageItemAdded.RemoveDynamic(this,&UPackageUserWidget::OnItemAdded);
		PackageComp->OnUserPackageItemRemoved.RemoveDynamic(this,&UPackageUserWidget::OnItemRemoved);
	}
	Super::NativeDestruct();
}

void UPackageUserWidget::OnPackageChanged()
{
	RefreshAllSlots();
}

void UPackageUserWidget::OnItemAdded(FName ItemID, int32 AddCount)
{
	RefreshAllSlots();
}

void UPackageUserWidget::OnItemRemoved(FName ItemID, int32 RemoveCount)
{
	RefreshAllSlots();
}

UItemDataAsset* UPackageUserWidget::FindItemDataByID(FName ItemID) const
{
	if (!PackageComp || ItemID.IsNone())
	{
		return nullptr;
	}
	//查询
	for (UItemDataAsset* Data : PackageComp->ItemDataBase)
	{
		if (Data && Data->ItemID == ItemID)
		{
			return Data;
		}
	}
	return nullptr;
}
