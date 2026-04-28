// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataAsset/ItemDataAsset.h"
#include "PackageUserWidget.generated.h"

class UUniformGridPanel;
class UPackageSlotUserWidget;
class UUserPackageComponent;

/**
 * 
 */
UCLASS()
class PACKAGECOMPONENT_API UPackageUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//初始化UMG并绑定Package
	UFUNCTION(BlueprintCallable,Category= "PackageUI")
	void InitWithPackageComponent(UUserPackageComponent* InPackageComponent);
	
	//手动触发 全量更新 （调试/外部触发）
	UFUNCTION(BlueprintCallable,Category= "PackageUI")
	void RefreshAllSlots();
	
protected:
	//构造完成后调用 初始化UI
	virtual void NativeConstruct() override;
	
	//销毁前调用 安全解绑代理
	virtual void NativeDestruct() override;
	
	//回调 Package任意代理触发 执行全量更新的回调
	UFUNCTION()
	void OnPackageChanged();
	
	//回调 Item新增 （执行全量更新）
	UFUNCTION()
	void OnItemAdded(FName ItemID,int32 AddCount);
	
	//回调 Item移除 （执行全量更新）
	UFUNCTION()
	void OnItemRemoved(FName ItemID,int32 RemoveCount);
	
	//通过ItemID配置
	UItemDataAsset* FindItemDataByID(FName ItemID) const;
	
	//网格排布
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Items = nullptr;
	
	//单格Widget（需在BP中指定）
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PackageUI")
	TSubclassOf<UPackageSlotUserWidget> SlotWidgetClass;
	
	//每行SlotWidget数量  固定为5
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PackageUI")
	int32 ColumnCount = 5;
	
	//绑定背包组件
	UPROPERTY(BlueprintReadOnly,Category="PackageUI")
	TObjectPtr<UUserPackageComponent> PackageComp = nullptr;
};
