// Copyright Epic Games, Inc. All Rights Reserved.

#include "PackageComponentGameMode.h"
#include "PackageComponentPlayerController.h"
#include "PackageComponentCharacter.h"
#include "PkgGameStateBase.h"
#include "Component/CountComponent.h"
#include "UObject/ConstructorHelpers.h"

APackageComponentGameMode::APackageComponentGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APackageComponentPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	
	GameStateClass = APkgGameStateBase::StaticClass();
}

void APackageComponentGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	APkgGameStateBase* GS = GetGameState<APkgGameStateBase>();
	if (GS)
	{
		GS->CountComponent->OnCountTick.AddDynamic(this, &APackageComponentGameMode::HandleCountTick);
		GS->CountComponent->OnCountFinished.AddDynamic(this, &APackageComponentGameMode::HandleCountFinished);
		GS->CountComponent->StartCount(StartCountDuration);
	}
}

void APackageComponentGameMode::TriggerStartCount(float InDurationSeconds)
{
	APkgGameStateBase* GS = GetGameState<APkgGameStateBase>();
	if (!GS || !GS->CountComponent)
	{
		return;
	}
	const float FinalDuration = (InDurationSeconds > 0.f) ? InDurationSeconds : GS->CountComponent->DurationSeconds;
	GS->CountComponent->StartCount(FinalDuration);
}

void APackageComponentGameMode::HandleCountTick(float RemainingSeconds)
{
	UE_LOG(LogTemp,Log,TEXT("RemainingSeconds:%f"),RemainingSeconds);
}

void APackageComponentGameMode::HandleCountFinished()
{
	UE_LOG(LogTemp,Log,TEXT("Count Finished"));
	APkgGameStateBase* GS = GetGameState<APkgGameStateBase>();
	if (GS)
	{
		GS->CountComponent->StopCount();
	}
}
