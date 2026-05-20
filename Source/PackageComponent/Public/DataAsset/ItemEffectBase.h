// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemEffectBase.generated.h"

/**
 * 
 */
//效果基类

//蓝图可继承，抽象类，允许在详细面板内联创建（不需要单独建asset），作为UPROPERTY成员时默认按实例对象存，而非共享同一个类默认对象引用
//EditInlineNew 可在面板直接创建
//DefaultToInstanced 创建后为独立对象而非共享
UCLASS(Blueprintable,Abstract,EditInlineNew,DefaultToInstanced)
class PACKAGECOMPONENT_API UItemEffectBase : public UObject
{
	GENERATED_BODY()
public:
	//执行前校验，确保目标有效
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="ItemEffect")
	bool CanApply(AActor* TargetActor) const;
	virtual bool CanApply_Implementation(AActor* TargetActor) const;
	
	//执行效果
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="ItemEffect")
	void ApplyEffect(AActor* TargetActor);
	virtual void ApplyEffect_Implementation(AActor* TargetActor);
};
