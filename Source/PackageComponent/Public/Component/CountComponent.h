// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CountComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountStart,float,TotalSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountTick,float,RemainingSecconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountStopped);

UCLASS( ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent) )
class PACKAGECOMPONENT_API UCountComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCountComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="Countdown")
	void StartCount(float InDurationSeconds);
	
	UFUNCTION(BlueprintCallable, Category="Countdown")
	void StopCount();
	
	UFUNCTION(BlueprintCallable, Category="Countdown")
	void ResetCount(float InDurationSeconds = -1.f);
	
	UFUNCTION(BlueprintPure, Category="Countdown")
	bool IsTimerRunning() const { return bIsTimerRunning; };
	
	UFUNCTION(BlueprintCallable, Category="Countdown")
	float GetTimerRemaining() const { return RemainingSeconds; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Countdown")
	bool bPauseGameWhenFinish = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Countdown")
	float DurationSeconds = 60.f;
	
	UPROPERTY(BlueprintAssignable, Category="Countdown | Event")
	FOnCountStart OnCountStart;
	
	UPROPERTY(BlueprintAssignable, Category="Countdown | Event")
	FOnCountFinished OnCountFinished;
	
	UPROPERTY(BlueprintAssignable, Category="Countdown | Event")
	FOnCountTick OnCountTick;
	
	UPROPERTY(BlueprintAssignable, Category="Countdown | Event")
	FOnCountStopped OnCountStopped;
private:
	UPROPERTY(VisibleAnywhere, Category="Countdown")
	float RemainingSeconds = 0.f;
	
	UPROPERTY(VisibleAnywhere, Category="Countdown")
	bool bIsTimerRunning = false;
	
	//保存此次定时器的句柄
	FTimerHandle CountTimerHandle;
	
	//回调函数 定时器触发时执行
	void HandleCountTickInternal();
	
	//回调函数 计时结束 执行广播
	void HandleCountFinishedInternal();
};
