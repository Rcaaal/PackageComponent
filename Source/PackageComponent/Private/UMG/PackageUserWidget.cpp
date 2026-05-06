// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/PackageUserWidget.h"

#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
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
	if (!Wrap_Items || !PackageComp || !SlotWidgetClass)
	{
		return;
	}
	
	Wrap_Items->ClearChildren();
	const int32 SafeColumnCount = FMath::Max(1, ColumnCount);
	const float WrapWidth = SlotSize * SafeColumnCount + SlotSpacing * (SafeColumnCount - 1);
	Wrap_Items->SetWrapSize(WrapWidth);
	Wrap_Items->SetInnerSlotPadding(FVector2D(SlotSpacing, SlotSpacing));
	Wrap_Items->SetOrientation(EOrientation::Orient_Horizontal);

	const TArray<FInventorySlot>& InventorySlots = PackageComp->GetSlots();
	
	for (const FInventorySlot& InventorySlot : InventorySlots)
	{
		if (!InventorySlot.bOccupied || InventorySlot.ItemInstance.ItemID.IsNone() || InventorySlot.ItemInstance.Count <= 0)
		{
			continue;
		}

		UItemDataAsset* ItemData = FindItemDataByID(InventorySlot.ItemInstance.ItemID);
		UTexture2D* IconTexture = nullptr;
		if (ItemData)
		{
			IconTexture = ItemData->Icon.LoadSynchronous();
		}

		UPackageSlotUserWidget* SlotWidget = CreateWidget<UPackageSlotUserWidget>(this,SlotWidgetClass);
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->SetSlotData(IconTexture,InventorySlot.ItemInstance.Count);

		if (UWrapBoxSlot* WrapSlot = Wrap_Items->AddChildToWrapBox(SlotWidget))
		{
			WrapSlot->SetPadding(FMargin(0.0f));
			WrapSlot->SetHorizontalAlignment(HAlign_Left);
			WrapSlot->SetVerticalAlignment(VAlign_Top);
			WrapSlot->SetFillEmptySpace(false);
			WrapSlot->SetFillSpanWhenLessThan(false);
		}
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
	for (UItemDataAsset* Data : PackageComp->ItemDataBase)
	{
		if (Data && Data->ItemID == ItemID)
		{
			return Data;
		}
	}
	return nullptr;
}
