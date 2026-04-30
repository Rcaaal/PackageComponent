// UserPackageComponent.cpp
#include "Package/UserPackageComponent.h"
#include "UI/PackagePanelWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UUserPackageComponent::UUserPackageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUserPackageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Slots.Num() != PackageSlotNum)
	{
		InitPackage(PackageSlotNum);
	}

	CreatePackageWidgetIfNeeded();
}

void UUserPackageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PackageWidgetInstance)
	{
		PackageWidgetInstance->RemoveFromParent();
		PackageWidgetInstance = nullptr;
	}

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

	for (int32 i = 0; i < Slots.Num(); ++i)
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

	for (FInventorySlot& Slot : Slots)
	{
		if (AddRemainingCount <= 0)
		{
			break;
		}
		if (!Slot.bOccupied)
		{
			continue;
		}
		if (Slot.ItemInstance.ItemID != ItemID)
		{
			continue;
		}

		const int32 RemainingSpace = CurrentItemMaxStackCount - Slot.ItemInstance.Count;
		if (RemainingSpace <= 0)
		{
			continue;
		}

		const int32 CurrentAddCount = FMath::Min(RemainingSpace, AddRemainingCount);
		Slot.ItemInstance.Count += CurrentAddCount;
		AddRemainingCount -= CurrentAddCount;
	}

	for (FInventorySlot& Slot : Slots)
	{
		if (AddRemainingCount <= 0)
		{
			break;
		}
		if (Slot.bOccupied)
		{
			continue;
		}

		const int32 CurrentAddCount = FMath::Min(CurrentItemMaxStackCount, AddRemainingCount);
		SetSlotItem(Slot, ItemID, CurrentAddCount);
		AddRemainingCount -= CurrentAddCount;
	}

	const int32 AddedCount = AddCount - AddRemainingCount;
	if (AddedCount > 0)
	{
		OnUserPackageComponentChanged.Broadcast();
		OnUserPackageItemAdded.Broadcast(ItemID, AddedCount);
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
		if (!Slot.bOccupied)
		{
			continue;
		}
		if (Slot.ItemInstance.ItemID != ItemID)
		{
			continue;
		}

		const int32 CurrentRemoveCount = FMath::Min(Slot.ItemInstance.Count, RemoveRemainingCount);
		Slot.ItemInstance.Count -= CurrentRemoveCount;
		RemoveRemainingCount -= CurrentRemoveCount;

		if (Slot.ItemInstance.Count <= 0)
		{
			ClearSlot(Slot);
		}
	}

	const int32 RemovedCount = RemoveCount - RemoveRemainingCount;
	if (RemovedCount > 0)
	{
		OnUserPackageComponentChanged.Broadcast();
		OnUserPackageItemRemoved.Broadcast(ItemID, RemovedCount);
	}

	return RemovedCount;
}

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

bool UUserPackageComponent::CanConsumeItem(FName ItemID, int32 ConsumeCount) const
{
	if (ConsumeCount <= 0)
	{
		return true;
	}
	return GetItemTotalCount(ItemID) >= ConsumeCount;
}

bool UUserPackageComponent::CanAddItem(FName ItemID, int32 AddCount) const
{
	if (ItemID.IsNone() || AddCount <= 0)
	{
		return false;
	}

	const int32 CurrentItemMaxStackCount = FMath::Max(1, GetMaxStackCount(ItemID));
	int32 RemainingCount = 0;

	for (const FInventorySlot& Slot : Slots)
	{
		if (!Slot.bOccupied)
		{
			RemainingCount += CurrentItemMaxStackCount;
			continue;
		}

		if (Slot.ItemInstance.ItemID == ItemID)
		{
			RemainingCount += FMath::Max(0, CurrentItemMaxStackCount - Slot.ItemInstance.Count);
		}
	}

	return RemainingCount >= AddCount;
}

int32 UUserPackageComponent::GetEmptySlotCount() const
{
	int32 EmptySlotCount = 0;
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

void UUserPackageComponent::TogglePackageUI()
{
	if (bPackageUIVisible)
	{
		HidePackageUI();
	}
	else
	{
		ShowPackageUI();
	}
}

void UUserPackageComponent::ShowPackageUI()
{
	CreatePackageWidgetIfNeeded();
	if (!PackageWidgetInstance)
	{
		return;
	}

	bPackageUIVisible = true;
	PackageWidgetInstance->SetVisibility(ESlateVisibility::Visible);

	if (APlayerController* PC = ResolveLocalPlayerController())
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
	PackageWidgetInstance->SetVisibility(HiddenVisibility);

	if (APlayerController* PC = ResolveLocalPlayerController())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

bool UUserPackageComponent::IsPackageUIVisible() const
{
	return bPackageUIVisible;
}

int32 UUserPackageComponent::GetMaxStackCount(FName ItemID) const
{
	for (const UItemDataAsset* Data : ItemDataBase)
	{
		if (!Data)
		{
			continue;
		}
		if (Data->ItemID == ItemID)
		{
			return FMath::Max(1, Data->MaxStack);
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
	Slot.ItemInstance.Count = FMath::Max(1, Count);
}

void UUserPackageComponent::CreatePackageWidgetIfNeeded()
{
	if (!bEnablePackageUI || PackageWidgetInstance || !PackageWidgetClass)
	{
		return;
	}

	APlayerController* PC = ResolveLocalPlayerController();
	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	PackageWidgetInstance = CreateWidget<UPackagePanelWidget>(PC, PackageWidgetClass);
	if (!PackageWidgetInstance)
	{
		return;
	}

	PackageWidgetInstance->AddToViewport();
	PackageWidgetInstance->SetVisibility(HiddenVisibility);

	// 初始化绑定组件代理
	PackageWidgetInstance->InitWithPackage(this);

	bPackageUIVisible = false;
}

APlayerController* UUserPackageComponent::ResolveLocalPlayerController() const
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
