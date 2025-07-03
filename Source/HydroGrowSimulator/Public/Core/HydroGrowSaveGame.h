#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/HydroGrowTypes.h"
#include "Systems/TimeManager.h"
#include "HydroGrowSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlantSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	FName PlantSpeciesID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	EPlantGrowthStage GrowthStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	float GrowthProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	float AgeInDays;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	float HealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	int32 ContainerIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Save Data")
	int32 SlotIndex;

	FPlantSaveData()
	{
		PlantSpeciesID = NAME_None;
		GrowthStage = EPlantGrowthStage::Seed;
		GrowthProgress = 0.0f;
		AgeInDays = 0.0f;
		HealthPoints = 100.0f;
		WorldLocation = FVector::ZeroVector;
		ContainerIndex = -1;
		SlotIndex = -1;
	}
};

USTRUCT(BlueprintType)
struct FContainerSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	EContainerType ContainerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	FRotator WorldRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	FEnvironmentalConditions EnvironmentalConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	FNutrientLevels NutrientLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	bool bPumpRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container Save Data")
	TArray<int32> PlantSlotIndices;

	FContainerSaveData()
	{
		ContainerType = EContainerType::DWC;
		WorldLocation = FVector::ZeroVector;
		WorldRotation = FRotator::ZeroRotator;
		bPumpRunning = false;
	}
};

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString ItemType;

	FInventoryItemData()
	{
		ItemID = NAME_None;
		Quantity = 0;
		ItemType = TEXT("Unknown");
	}

	FInventoryItemData(FName InItemID, int32 InQuantity, const FString& InItemType)
	{
		ItemID = InItemID;
		Quantity = InQuantity;
		ItemType = InItemType;
	}
};

USTRUCT(BlueprintType)
struct FSkillTreeSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	TMap<FString, int32> UnlockedSkills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	int32 AvailableSkillPoints;

	FSkillTreeSaveData()
	{
		AvailableSkillPoints = 0;
	}
};

USTRUCT(BlueprintType)
struct FChallengeSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenges")
	TArray<FString> CompletedDailyChallenges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenges")
	TArray<FString> CompletedWeeklyChallenges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenges")
	TArray<FString> ActiveChallenges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenges")
	FDateTime LastDailyChallengeReset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenges")
	FDateTime LastWeeklyChallengeReset;
};

UCLASS()
class HYDROGROWSIMULATOR_API UHydroGrowSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UHydroGrowSaveGame();

	// Player Progress
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Progress")
	int32 PlayerLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Progress")
	int32 Experience;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Progress")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Progress")
	int32 ResearchPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Progress")
	int32 EnergyCredits;

	// Game State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	FGameDateTime GameTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	EGameTimeMode TimeMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	FDateTime RealTimeSaved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	float TotalPlayTimeHours;

	// Facility Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Facility")
	TArray<FContainerSaveData> Containers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Facility")
	TArray<FPlantSaveData> Plants;

	// Inventory and Items
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItemData> Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<FName, int32> SeedInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<FName, int32> EquipmentInventory;

	// Progression Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	FSkillTreeSaveData SkillTreeData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	TArray<FName> UnlockedPlants;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	TArray<FName> UnlockedEquipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	TArray<FString> UnlockedAchievements;

	// Statistics
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Statistics")
	FGameplayStats LifetimeStats;

	// Challenges
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Challenges")
	FChallengeSaveData ChallengeData;

	// Settings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	TMap<FString, float> AudioSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	int32 GraphicsQuality;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	TMap<FString, FString> GameplaySettings;

	// Save Game Management
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
	int32 SaveVersion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save System")
	int32 UserIndex;

public:
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void InitializeNewSave();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool IsValidSave() const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void UpdatePlayTime(float AdditionalHours);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddInventoryItem(FName ItemID, int32 Quantity, const FString& ItemType);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveInventoryItem(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetInventoryItemCount(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void UnlockPlant(FName PlantID);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void UnlockEquipment(FName EquipmentID);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void UnlockAchievement(const FString& AchievementID);

	UFUNCTION(BlueprintPure, Category = "Progression")
	bool IsPlantUnlocked(FName PlantID) const;

	UFUNCTION(BlueprintPure, Category = "Progression")
	bool IsEquipmentUnlocked(FName EquipmentID) const;

	UFUNCTION(BlueprintPure, Category = "Progression")
	bool IsAchievementUnlocked(const FString& AchievementID) const;

private:
	static constexpr int32 CURRENT_SAVE_VERSION = 1;
};