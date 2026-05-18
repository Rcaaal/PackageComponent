// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/BaseUserWidget.h"

#include "PkgGameStateBase.h"
#include "Component/CountComponent.h"
#include "Components/TextBlock.h"

void UBaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APkgGameStateBase* GS = GetWorld()->GetGameState<APkgGameStateBase>())
	{
		CachedCountComponent = GS->CountComponent;
		if (CachedCountComponent)
		{
			CachedCountComponent->OnCountTick.AddDynamic(this, &UBaseUserWidget::HandleCountTick);
			CachedCountComponent->OnCountFinished.AddDynamic(this, &UBaseUserWidget::HandleCountFinished);
			
			HandleCountTick(CachedCountComponent->GetTimerRemaining());
		}
	}
}

void UBaseUserWidget::NativeDestruct()
{
	if (CachedCountComponent)
	{
		CachedCountComponent->OnCountTick.RemoveDynamic(this, &UBaseUserWidget::HandleCountTick);
		CachedCountComponent->OnCountFinished.RemoveDynamic(this, &UBaseUserWidget::HandleCountFinished);
	}
	Super::NativeDestruct();
}

void UBaseUserWidget::HandleCountTick(float RemainingSeconds)
{
	if (TxtCount)
	{
		TxtCount->SetText(FText::Format(FText::FromString(TEXT("Time:{0}s")),FText::AsNumber(FMath::CeilToInt(RemainingSeconds))));
	}
}

void UBaseUserWidget::HandleCountFinished()
{
	if (TxtCount)
	{
		TxtCount->SetText(FText::FromString(TEXT("Time: 0")));
	}
}
