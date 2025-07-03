#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "HydroGrowPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class APlantActor;
class AHydroponicsContainer;

UCLASS()
class HYDROGROWSIMULATOR_API AHydroGrowPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHydroGrowPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SelectPlant(APlantActor* Plant);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SelectContainer(AHydroponicsContainer* Container);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleGameSpeed();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenPlantInfoPanel();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenShopPanel();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenInventoryPanel();

protected:
	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CameraRotateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CameraZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SelectObjectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ContextMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* QuickHarvestAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleSpeedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* OpenInventoryAction;

	// Input Callbacks
	void CameraRotate(const FInputActionValue& Value);
	void CameraZoom(const FInputActionValue& Value);
	void SelectObject(const FInputActionValue& Value);
	void OpenContextMenu(const FInputActionValue& Value);
	void QuickHarvest(const FInputActionValue& Value);
	void ToggleSpeed(const FInputActionValue& Value);
	void OpenInventory(const FInputActionValue& Value);

private:
	UPROPERTY()
	AActor* CameraTarget;

	UPROPERTY()
	APlantActor* SelectedPlant;

	UPROPERTY()
	AHydroponicsContainer* SelectedContainer;

	// Camera settings
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraRotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraZoomSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MinZoomDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MaxZoomDistance;

	// Current camera state
	float CurrentCameraDistance;
	FRotator CameraRotation;

	void UpdateCameraPosition();
	AActor* GetActorUnderCursor() const;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantSelected, APlantActor*, Plant);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContainerSelected, AHydroponicsContainer*, Container);

	UPROPERTY(BlueprintAssignable)
	FOnPlantSelected OnPlantSelected;

	UPROPERTY(BlueprintAssignable)
	FOnContainerSelected OnContainerSelected;
};