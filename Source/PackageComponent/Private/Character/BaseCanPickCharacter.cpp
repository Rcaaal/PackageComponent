// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCanPickCharacter.h"

#include "Blueprint/UserWidget.h"

void ABaseCanPickCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (InteractWidgetClass)
	{
		InteractWidget = CreateWidget<UUserWidget>(GetWorld(), InteractWidgetClass);
		if (InteractWidget)
		{
			InteractWidget->AddToViewport();
			InteractWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABaseCanPickCharacter::ShowInteractWidget(bool bShow)
{
	if (!InteractWidget)
	{
		return;
	}
	InteractWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ABaseCanPickCharacter::SetCurrentPick(APickupActorBase* InPickup)
{
	CurrentPick = InPickup;
	ShowInteractWidget(CurrentPick != nullptr);
}

void ABaseCanPickCharacter::ClearCurrentPick(APickupActorBase* InPickup)
{
	if (CurrentPick == InPickup)
	{
		CurrentPick = nullptr;
		ShowInteractWidget(false);
	}
}

void ABaseCanPickCharacter::TryPickupNow()
{
	if (!CurrentPick)
	{
		return;
	}
	CurrentPick->TryPickup(this);
}


