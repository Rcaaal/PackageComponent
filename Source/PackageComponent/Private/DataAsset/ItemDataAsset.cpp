// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/ItemDataAsset.h"

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	const FName AssetName = ItemID.IsNone() ? GetFName() : ItemID;
	return FPrimaryAssetId(TEXT("Item"), AssetName);
}
