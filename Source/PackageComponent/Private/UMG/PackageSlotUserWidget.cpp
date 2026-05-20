// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/PackageSlotUserWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPackageSlotUserWidget::SetSlotData(int32 InSlotIndex,UTexture2D* InIcon, int32 InCount)
{
	CachedSlotIndex = InSlotIndex;
	if (Img_ItemIcon)
	{
		Img_ItemIcon->SetBrushFromTexture(InIcon,true);
	}
	if (Text_Count)
	{
		const int32 SafeCount = FMath::Max(0,InCount);
		Text_Count->SetText(FText::AsNumber(SafeCount));
	}
}

void UPackageSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_UseItem)
	{
		//绑定鼠标左键被按下时的回调
		Button_UseItem->OnClicked.AddDynamic(this,&UPackageSlotUserWidget::HandleUserButtonClicked);
	}
}

FReply UPackageSlotUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (CachedSlotIndex != INDEX_NONE)
		{
			RightMouseButtonClick.Broadcast(CachedSlotIndex);
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, MouseEvent);
}

void UPackageSlotUserWidget::HandleUserButtonClicked()
{
	if (CachedSlotIndex != INDEX_NONE)
	{
		LeftMouseButtonClick.Broadcast(CachedSlotIndex);
	}
}
