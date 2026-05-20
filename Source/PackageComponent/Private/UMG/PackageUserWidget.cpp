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

		UItemDataAsset* ItemData = PackageComp->FindItemDataByID(InventorySlot.ItemInstance.ItemID);
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

		SlotWidget->SetSlotData(InventorySlot.SlotIndex,IconTexture,InventorySlot.ItemInstance.Count);
		
		SlotWidget->LeftMouseButtonClick.AddDynamic(this,&UPackageUserWidget::LeftButtonClick);
		SlotWidget->RightMouseButtonClick.AddDynamic(this,&UPackageUserWidget::RightButtonClick);

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

//bool UPackageUserWidget::DropItem(FName ItemID, int32 DropCount, FVector DropOrigin)
//{
//	if (ItemID.IsNone() || DropCount <= 0 || !PickupActorClass)
//	{
//		return false;
//	}
//	if ()
//	{
//		
//	}
//}

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

void UPackageUserWidget::LeftButtonClick(int32 SlotIndex)
{
	if (!PackageComp)
	{
		return;
	}

	const TArray<FInventorySlot>& SlotsRef = PackageComp->GetSlots();
	if (!SlotsRef.IsValidIndex(SlotIndex))
	{
		return;
	}

	const FInventorySlot& SlotData = SlotsRef[SlotIndex];
	if (!SlotData.bOccupied || SlotData.ItemInstance.ItemID.IsNone() || SlotData.ItemInstance.ItemID.IsNone())
	{
		return;
	}
	if (AActor* TargetActor= GetOwningPlayerPawn())
	{
		PackageComp->UseItem(SlotData.ItemInstance.ItemID,TargetActor);
	}
}

void UPackageUserWidget::RightButtonClick(int32 SlotIndex)
{
	if (!PackageComp)
	{
		return;
	}

	const TArray<FInventorySlot>& SlotsRef = PackageComp->GetSlots();
	if (!SlotsRef.IsValidIndex(SlotIndex))
	{
		return;
	}

	const FInventorySlot& InventorySlotData = SlotsRef[SlotIndex];
	if (!InventorySlotData.bOccupied || InventorySlotData.ItemInstance.ItemID.IsNone())
	{
		return;
	}

	const FName ItemID = InventorySlotData.ItemInstance.ItemID;
	const int32 DropCount = 1;
	
	
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		PackageComp->DropItem(ItemID, DropCount, OwnerPawn->GetActorLocation());
	}
}
