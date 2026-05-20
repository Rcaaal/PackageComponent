// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CountComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCountComponent::UCountComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCountComponent::BeginPlay()
{
	Super::BeginPlay();
	RemainingSeconds = DurationSeconds;
	// ...
	
}


// Called every frame
void UCountComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCountComponent::StartCount(float InDurationSeconds)
{
	if (InDurationSeconds > 0.f)
	{
		DurationSeconds = InDurationSeconds;
	}
	if (DurationSeconds<=0.f)
	{
		return;
	}
	RemainingSeconds = DurationSeconds;
	bIsTimerRunning = true;
	OnCountStart.Broadcast(DurationSeconds);
	OnCountTick.Broadcast(RemainingSeconds);

	GetWorld()->GetTimerManager().ClearTimer(CountTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CountTimerHandle, this, &UCountComponent::HandleCountTickInternal, 0.1f, true);
}

void UCountComponent::StopCount()
{
	if (!bIsTimerRunning)
	{
		return;
	}
	bIsTimerRunning = false;
	GetWorld()->GetTimerManager().ClearTimer(CountTimerHandle);
	OnCountStopped.Broadcast();
}

void UCountComponent::ResetCount(float InDurationSeconds)
{
	StopCount();
	if (InDurationSeconds > 0.f)
	{
		DurationSeconds = InDurationSeconds;
	}
	RemainingSeconds = DurationSeconds;
	OnCountTick.Broadcast(RemainingSeconds);
}

void UCountComponent::HandleCountTickInternal()
{
	RemainingSeconds = FMath::Max(0.f,RemainingSeconds - 0.1f );
	OnCountTick.Broadcast(RemainingSeconds);
	if (RemainingSeconds <= KINDA_SMALL_NUMBER)
	{
		HandleCountFinishedInternal();
	}
}

void UCountComponent::HandleCountFinishedInternal()
{
	StopCount();
	RemainingSeconds = 0.f;
	OnCountTick.Broadcast(RemainingSeconds);
	OnCountFinished.Broadcast();
	if (bPauseGameWhenFinish)
	{
		UGameplayStatics::SetGamePaused(GetWorld(),true);
	}
}

