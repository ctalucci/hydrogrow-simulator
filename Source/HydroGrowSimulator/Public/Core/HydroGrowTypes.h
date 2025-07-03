#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HydroGrowTypes.generated.h"

UENUM(BlueprintType)
enum class EContainerType : uint8
{
	None			UMETA(DisplayName = "None"),
	DWC				UMETA(DisplayName = "Deep Water Culture"),
	EbbFlow			UMETA(DisplayName = "Ebb & Flow"),
	NFT				UMETA(DisplayName = "Nutrient Film Technique"),
	Aeroponics		UMETA(DisplayName = "Aeroponics"),
	DripSystem		UMETA(DisplayName = "Drip System")
};

UENUM(BlueprintType)
enum class EPlantGrowthStage : uint8
{
	Seed			UMETA(DisplayName = "Seed"),
	Seedling		UMETA(DisplayName = "Seedling"),
	Vegetative		UMETA(DisplayName = "Vegetative"),
	Flowering		UMETA(DisplayName = "Flowering"),
	Harvest			UMETA(DisplayName = "Ready for Harvest"),
	Dead			UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EPlantType : uint8
{
	Leafy			UMETA(DisplayName = "Leafy Greens"),
	Fruiting		UMETA(DisplayName = "Fruiting Plants"),
	Herbs			UMETA(DisplayName = "Herbs"),
	Root			UMETA(DisplayName = "Root Vegetables"),
	Flowering		UMETA(DisplayName = "Flowering Plants")
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None			UMETA(DisplayName = "None"),
	Light			UMETA(DisplayName = "Lighting"),
	Pump			UMETA(DisplayName = "Pump"),
	Sensor			UMETA(DisplayName = "Sensor"),
	Controller		UMETA(DisplayName = "Controller"),
	Filter			UMETA(DisplayName = "Filter"),
	Heater			UMETA(DisplayName = "Heater"),
	Fan				UMETA(DisplayName = "Fan")
};

USTRUCT(BlueprintType)
struct FPlantSpeciesData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
	EPlantType PlantType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
	float GrowthTimeInDays;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
	FVector2D OptimalPHRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
	FVector2D OptimalECRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
	FVector2D OptimalTemperatureRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
	float OptimalHumidity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
	float LightHoursRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 BaseYield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 MarketValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 SeedCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
	int32 UnlockLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UStaticMesh> PlantMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UMaterialInterface> PlantMaterial;

	FPlantSpeciesData()
	{
		DisplayName = FText::FromString("Unknown Plant");
		Description = FText::FromString("A mysterious plant species");
		PlantType = EPlantType::Leafy;
		GrowthTimeInDays = 7.0f;
		OptimalPHRange = FVector2D(5.5f, 6.5f);
		OptimalECRange = FVector2D(1.0f, 2.0f);
		OptimalTemperatureRange = FVector2D(18.0f, 24.0f);
		OptimalHumidity = 60.0f;
		LightHoursRequired = 14.0f;
		BaseYield = 1;
		MarketValue = 10;
		SeedCost = 5;
		UnlockLevel = 1;
	}
};

USTRUCT(BlueprintType)
struct FEnvironmentalConditions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float PHLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float ECLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float Humidity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float LightIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float OxygenLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float WaterLevel;

	FEnvironmentalConditions()
	{
		PHLevel = 6.0f;
		ECLevel = 1.5f;
		Temperature = 21.0f;
		Humidity = 60.0f;
		LightIntensity = 1.0f;
		OxygenLevel = 1.0f;
		WaterLevel = 1.0f;
	}
};

USTRUCT(BlueprintType)
struct FNutrientLevels
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Nutrients")
	float Nitrogen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Nutrients")
	float Phosphorus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Nutrients")
	float Potassium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Secondary Nutrients")
	float Calcium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Secondary Nutrients")
	float Magnesium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Secondary Nutrients")
	float Sulfur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Micronutrients")
	float Iron;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Micronutrients")
	float Manganese;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Micronutrients")
	float Zinc;

	FNutrientLevels()
	{
		Nitrogen = 1.0f;
		Phosphorus = 1.0f;
		Potassium = 1.0f;
		Calcium = 1.0f;
		Magnesium = 1.0f;
		Sulfur = 1.0f;
		Iron = 1.0f;
		Manganese = 1.0f;
		Zinc = 1.0f;
	}
	bool IsValid() const
	{
		return Nitrogen >= 0 && Phosphorus >= 0 && Potassium >= 0 &&
			   Calcium >= 0 && Magnesium >= 0 && Sulfur >= 0 &&
			   Iron >= 0 && Manganese >= 0 && Zinc >= 0;
	}
};

USTRUCT(BlueprintType)
struct FEquipmentData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float PowerConsumption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Effectiveness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 PurchaseCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 MaintenanceCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
	int32 UnlockLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UStaticMesh> EquipmentMesh;

	FEquipmentData()
	{
		DisplayName = FText::FromString("Basic Equipment");
		Description = FText::FromString("Standard hydroponics equipment");
		EquipmentType = EEquipmentType::None;
		PowerConsumption = 10.0f;
		Effectiveness = 1.0f;
		PurchaseCost = 100;
		MaintenanceCost = 5;
		UnlockLevel = 1;
	}

	bool Isvalid() const
	{
		return EquipmentType != EEquipmentType::None && PurchaseCost > 0 && MaintenanceCost >= 0;
	}
};

USTRUCT(BlueprintType)
struct FGameplayStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 TotalPlantsGrown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 SuccessfulHarvests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 PlantsLost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 TotalCoinsEarned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 TotalCoinsSpent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TotalPlayTimeHours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenges")
	int32 ChallengesCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
	TArray<FString> UnlockedAchievements;

	FGameplayStats()
	{
		TotalPlantsGrown = 0;
		SuccessfulHarvests = 0;
		PlantsLost = 0;
		TotalCoinsEarned = 0;
		TotalCoinsSpent = 0;
		TotalPlayTimeHours = 0.0f;
		ChallengesCompleted = 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantGrowthStageChanged, EPlantGrowthStage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantHarvested, int32, Yield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnvironmentalChange, FString, Parameter, float, NewValue);