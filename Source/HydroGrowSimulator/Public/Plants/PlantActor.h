#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/HydroGrowTypes.h"
#include "Network/HydroGrowNetworkTypes.h"
#include "Components/ProceduralMeshComponent.h"
#include "PlantActor.generated.h"

class UHydroGrowGameInstance;
class AHydroponicsContainer;
class UTimeManager;

UCLASS()
class HYDROGROWSIMULATOR_API APlantActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APlantActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Plant")
	void InitializePlant(FName PlantSpeciesID, AHydroponicsContainer* Container);

	UFUNCTION(BlueprintCallable, Category = "Plant")
	bool CanHarvest() const;

	UFUNCTION(BlueprintCallable, Category = "Plant")
	int32 Harvest();

	UFUNCTION(BlueprintCallable, Category = "Plant")
	void WaterPlant(float WaterAmount);

	UFUNCTION(BlueprintCallable, Category = "Plant")
	void ApplyNutrients(const FNutrientLevels& Nutrients);

	// Network functions
	HYDROGROW_REPLICATE_FUNCTION_PARAMS(HarvestPlant, const FString&, PlayerName)
	HYDROGROW_REPLICATE_FUNCTION_PARAMS(WaterPlant, float, WaterAmount, const FString&, PlayerName)
	HYDROGROW_REPLICATE_FUNCTION_PARAMS(ApplyNutrients, const FNutrientLevels&, Nutrients, const FString&, PlayerName)

	HYDROGROW_MULTICAST_FUNCTION_PARAMS(PlantGrowthStageChanged, EPlantGrowthStage, NewStage)
	HYDROGROW_MULTICAST_FUNCTION_PARAMS(PlantHarvested, int32, Yield, const FString&, PlayerName)
	HYDROGROW_MULTICAST_FUNCTION_PARAMS(PlantHealthChanged, float, NewHealth)

	UFUNCTION(BlueprintPure, Category = "Plant")
	float GetGrowthPercentage() const;

	UFUNCTION(BlueprintPure, Category = "Plant")
	EPlantGrowthStage GetCurrentGrowthStage() const { return CurrentGrowthStage; }

	UFUNCTION(BlueprintPure, Category = "Plant")
	float GetHealthPercentage() const;

	UFUNCTION(BlueprintPure, Category = "Plant")
	FName GetPlantSpeciesID() const { return PlantSpeciesID; }

	UFUNCTION(BlueprintPure, Category = "Plant")
	const FPlantSpeciesData* GetPlantData() const;

	UFUNCTION(BlueprintPure, Category = "Plant")
	bool IsAlive() const { return CurrentGrowthStage != EPlantGrowthStage::Dead; }

	UFUNCTION(BlueprintPure, Category = "Plant")
	float GetDaysOld() const { return AgeInDays; }

	UFUNCTION(BlueprintCallable, Category = "Plant")
	void SetEnvironmentalConditions(const FEnvironmentalConditions& Conditions);

protected:
	// Core Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProceduralMeshComponent* PlantMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PotMesh;

	// Plant Data
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Plant Data")
	FName PlantSpeciesID;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Plant State")
	EPlantGrowthStage CurrentGrowthStage;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Plant State")
	float GrowthProgress;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Plant State")
	float AgeInDays;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Plant State")
	float HealthPoints;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Plant State")
	float MaxHealthPoints;

	// Network tracking
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	FString LastActionPlayer;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	FDateTime LastActionTime;

	// Environmental factors
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
	FEnvironmentalConditions CurrentEnvironment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
	FNutrientLevels CurrentNutrients;

	// Growth calculation factors
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Growth Factors")
	float PHEffectiveness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Growth Factors")
	float NutrientEffectiveness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Growth Factors")
	float LightEffectiveness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Growth Factors")
	float TemperatureEffectiveness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Growth Factors")
	float OverallGrowthRate;

	// Container reference
	UPROPERTY()
	AHydroponicsContainer* ParentContainer;

	// Subsystem references
	UPROPERTY()
	UHydroGrowGameInstance* GameInstance;

	UPROPERTY()
	UTimeManager* TimeManager;

	// Replication callbacks
	UFUNCTION()
	void OnRep_GrowthStage();

	UFUNCTION()
	void OnRep_HealthPoints();

	UFUNCTION()
	void OnRep_GrowthProgress();

private:
	void UpdateGrowthProgress(float DeltaTime);
	void UpdateGrowthStage();
	void UpdateHealthPoints(float DeltaTime);
	void CalculateGrowthFactors();
	void UpdateVisualAppearance();
	void CheckForProblems();

	float CalculatePHEffect(float CurrentPH, const FVector2D& OptimalRange) const;
	float CalculateNutrientEffect(float CurrentEC, const FVector2D& OptimalRange) const;
	float CalculateLightEffect(float LightIntensity, float RequiredHours) const;
	float CalculateTemperatureEffect(float CurrentTemp, const FVector2D& OptimalRange) const;

	// Growth stage thresholds
	static constexpr float SeedlingThreshold = 0.1f;
	static constexpr float VegetativeThreshold = 0.25f;
	static constexpr float FloweringThreshold = 0.6f;
	static constexpr float HarvestThreshold = 0.8f;

public:
	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnPlantGrowthStageChanged OnGrowthStageChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlantHarvested OnPlantHarvested;
};