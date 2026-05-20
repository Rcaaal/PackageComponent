// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PackageSlotUserWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
class UButton;
/**
 * 
 */

//左键点击回调
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeftMouseButtonClick,int32,SlotIndex);
//右键点击回调
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightMouseButtonClick,int32,SlotIndex);
UCLASS()
class PACKAGECOMPONENT_API UPackageSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//初始化格子显示内容
	UFUNCTION(BlueprintCallable,Category="PackageUI")
	void SetSlotData(int32 InSlotIndex,UTexture2D* InIcon,int32 InCount);
	
	UPROPERTY(BlueprintAssignable,Category="PackageUI|ButtonEvent")
	FOnLeftMouseButtonClick LeftMouseButtonClick;
	
	UPROPERTY(BlueprintAssignable,Category="PackageUI|ButtonEvent")
	FOnRightMouseButtonClick RightMouseButtonClick;
protected:
	virtual void NativeConstruct() override;
	//重写鼠标按下的回调事件
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;
	
	UFUNCTION()
	void HandleUserButtonClicked();
	
	//图标
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_ItemIcon = nullptr;
	
	//数量
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count = nullptr;
	
	//按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_UseItem = nullptr;
	
private:
	UPROPERTY()
	int32 CachedSlotIndex = INDEX_NONE;
};
