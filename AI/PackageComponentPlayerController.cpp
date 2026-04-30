// PackageComponentPlayerController.cpp
#include "PackageComponentPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "Package/UserPackageComponent.h"

void APackageComponentPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Default)
			{
				Subsys->AddMappingContext(IMC_Default, 0);
			}
		}
	}
}

void APackageComponentPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_TogglePackage)
		{
			EIC->BindAction(IA_TogglePackage, ETriggerEvent::Started, this, &APackageComponentPlayerController::OnTogglePackage);
		}
	}
}

void APackageComponentPlayerController::OnTogglePackage(const FInputActionValue& Value)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	if (UUserPackageComponent* PackageComp = MyPawn->FindComponentByClass<UUserPackageComponent>())
	{
		PackageComp->TogglePackageUI();
	}
}
