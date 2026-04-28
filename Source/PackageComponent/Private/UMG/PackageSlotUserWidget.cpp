// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/PackageSlotUserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPackageSlotUserWidget::SetSlotData(UTexture2D* InIcon, int32 InCount)
{
	if (Img_ItemIcon)
	{
		Img_ItemIcon->SetBrushFromTexture(InIcon,true);
	}
	if (Text_Count)
	{
		Text_Count->SetText(FText::AsNumber(FMath::Max(0,InCount)));
	}
}
