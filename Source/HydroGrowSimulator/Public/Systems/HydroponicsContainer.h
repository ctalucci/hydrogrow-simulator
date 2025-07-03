#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/HydroGrowTypes.h"
#include "HydroponicsContainer.generated.h"

class APlantActor;
class UStaticMeshComponent;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FPlantSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOccupied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlantActor* PlantActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SlotLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIndex;

	FPlantSlot()
	{
		bIsOccupied = false;
		PlantActor = nullptr;
		SlotLocation = FVector::ZeroVector;
		SlotIndex = -1;
	}
};

UCLASS()
class HYDROGROWSIMULATOR_API AHydroponicsContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	AHydroponicsContainer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	void InitializeContainer(EContainerType Type, int32 PlantCapacity);

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool CanPlantSeed(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool PlantSeed(FName PlantSpeciesID, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool RemovePlant(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Container")
	int32 GetAvailableSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	void SetPHLevel(float NewPH);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void SetECLevel(float NewEC);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void SetWaterLevel(float NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void AddNutrients(const FNutrientLevels& Nutrients);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void StartWaterPump();

	UFUNCTION(BlueprintCallable, Category = "Container")
	void StopWaterPump();

	UFUNCTION(BlueprintPure, Category = "Container")
	EContainerType GetContainerType() const { return ContainerType; }

	UFUNCTION(BlueprintPure, Category = "Container")
	const FEnvironmentalConditions& GetEnvironmentalConditions() const { return CurrentConditions; }

	UFUNCTION(BlueprintPure, Category = "Container")
	int32 GetPlantCount() const;

	UFUNCTION(BlueprintPure, Category = "Container")
	int32 GetMaxCapacity() const { return PlantSlots.Num(); }

	UFUNCTION(BlueprintPure, Category = "Container")
	bool IsPumpRunning() const { return bPumpRunning; }

	UFUNCTION(BlueprintPure, Category = "Container")
	float GetEnergyConsumption() const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	TArray<APlantActor*> GetAllPlants() const;

protected:
	// Core Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ContainerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WaterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionBox;

	// Container Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Setup")
	EContainerType ContainerType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Container Setup")
	TArray<FPlantSlot> PlantSlots;

	// Environmental Conditions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
	FEnvironmentalConditions CurrentConditions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
	FNutrientLevels NutrientSolution;

	// System State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
	bool bPumpRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
	float WaterFlowRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
	float EnergyConsumptionRate;

	// Configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float BaseEnergyConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float PumpEnergyConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float WaterCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TMap<EContainerType, float> TypeEfficiencyMultipliers;

private:
	void UpdateEnvironmentalConditions(float DeltaTime);
	void UpdateWaterSystem(float DeltaTime);
	void UpdateNutrientDistribution();
	void UpdatePlantConditions();
	void CreatePlantSlots(int32 Capacity);
	void UpdateVisualEffects();

	// Environmental drift simulation
	void SimulatePHDrift(float DeltaTime);
	void SimulateNutrientDepletion(float DeltaTime);
	void SimulateWaterEvaporation(float DeltaTime);

public:
	// Delegates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantAdded, APlantActor*, Plant);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantRemoved, int32, SlotIndex);

	UPROPERTY(BlueprintAssignable)
	FOnPlantAdded OnPlantAdded;

	UPROPERTY(BlueprintAssignable)
	FOnPlantRemoved OnPlantRemoved;

	UPROPERTY(BlueprintAssignable)
	FOnEnvironmentalChange OnEnvironmentalChange;
};