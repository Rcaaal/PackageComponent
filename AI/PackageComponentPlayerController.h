// PackageComponentPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PackageComponentPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PACKAGECOMPONENT_API APackageComponentPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Default = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_TogglePackage = nullptr;

private:
	void OnTogglePackage(const FInputActionValue& Value);
};
