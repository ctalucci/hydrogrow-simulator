#include "Core/HydroGrowSaveGame.h"
#include "Engine/Engine.h"

UHydroGrowSaveGame::UHydroGrowSaveGame()
{
	SaveSlotName = TEXT("HydroGrowSave");
	SaveVersion = CURRENT_SAVE_VERSION;
	UserIndex = 0;
	
	InitializeNewSave();
}

void UHydroGrowSaveGame::InitializeNewSave()
{
	// Initialize player progress
	PlayerLevel = 1;
	Experience = 0;
	Coins = 100;
	ResearchPoints = 0;
	EnergyCredits = 1000;
	
	// Initialize game state
	GameTime = FGameDateTime();
	TimeMode = EGameTimeMode::Normal;
	RealTimeSaved = FDateTime::Now();
	TotalPlayTimeHours = 0.0f;
	
	// Clear facility data
	Containers.Empty();
	Plants.Empty();
	
	// Initialize inventory
	Inventory.Empty();
	SeedInventory.Empty();
	EquipmentInventory.Empty();
	
	// Add starting seeds
	SeedInventory.Add(TEXT("Lettuce"), 5);
	SeedInventory.Add(TEXT("Basil"), 3);
	SeedInventory.Add(TEXT("Spinach"), 3);
	
	// Initialize progression
	SkillTreeData = FSkillTreeSaveData();
	UnlockedPlants.Empty();
	UnlockedEquipment.Empty();
	UnlockedAchievements.Empty();
	
	// Unlock starting plants
	UnlockedPlants.Add(TEXT("Lettuce"));
	UnlockedPlants.Add(TEXT("Basil"));
	UnlockedPlants.Add(TEXT("Spinach"));
	
	// Unlock basic equipment
	UnlockedEquipment.Add(TEXT("BasicContainer"));
	UnlockedEquipment.Add(TEXT("BasicLight"));
	UnlockedEquipment.Add(TEXT("BasicNutrients"));
	
	// Initialize statistics
	LifetimeStats = FGameplayStats();
	
	// Initialize challenge data
	ChallengeData = FChallengeSaveData();
	ChallengeData.LastDailyChallengeReset = FDateTime::Now();
	ChallengeData.LastWeeklyChallengeReset = FDateTime::Now();
	
	// Initialize settings
	AudioSettings.Empty();
	AudioSettings.Add(TEXT("Master"), 1.0f);
	AudioSettings.Add(TEXT("SFX"), 1.0f);
	AudioSettings.Add(TEXT("Music"), 0.7f);
	AudioSettings.Add(TEXT("Ambient"), 0.8f);
	
	GraphicsQuality = 2; // Medium quality
	
	GameplaySettings.Empty();
	GameplaySettings.Add(TEXT("AutoSave"), TEXT("true"));
	GameplaySettings.Add(TEXT("ShowTutorials"), TEXT("true"));
	GameplaySettings.Add(TEXT("ShowNotifications"), TEXT("true"));
	
	UE_LOG(LogTemp, Warning, TEXT("Initialized new save game"));
}

bool UHydroGrowSaveGame::IsValidSave() const
{
	// Basic validation checks
	if (SaveVersion <= 0 || SaveVersion > CURRENT_SAVE_VERSION)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid save version: %d"), SaveVersion);
		return false;
	}
	
	if (PlayerLevel < 1 || PlayerLevel > 100)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid player level: %d"), PlayerLevel);
		return false;
	}
	
	if (Coins < 0 || Experience < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Negative currency values"));
		return false;
	}
	
	return true;
}

void UHydroGrowSaveGame::UpdatePlayTime(float AdditionalHours)
{
	TotalPlayTimeHours += AdditionalHours;
	RealTimeSaved = FDateTime::Now();
}

void UHydroGrowSaveGame::AddInventoryItem(FName ItemID, int32 Quantity, const FString& ItemType)
{
	if (Quantity <= 0)
	{
		return;
	}
	
	// Check if item already exists
	for (FInventoryItemData& Item : Inventory)
	{
		if (Item.ItemID == ItemID && Item.ItemType == ItemType)
		{
			Item.Quantity += Quantity;
			UE_LOG(LogTemp, Log, TEXT("Added %d %s to inventory (total: %d)"), 
				Quantity, *ItemID.ToString(), Item.Quantity);
			return;
		}
	}
	
	// Add new item
	Inventory.Add(FInventoryItemData(ItemID, Quantity, ItemType));
	UE_LOG(LogTemp, Log, TEXT("Added new item to inventory: %d %s"), 
		Quantity, *ItemID.ToString());
}

bool UHydroGrowSaveGame::RemoveInventoryItem(FName ItemID, int32 Quantity)
{
	if (Quantity <= 0)
	{
		return false;
	}
	
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].ItemID == ItemID)
		{
			if (Inventory[i].Quantity >= Quantity)
			{
				Inventory[i].Quantity -= Quantity;
				
				// Remove item if quantity reaches zero
				if (Inventory[i].Quantity == 0)
				{
					Inventory.RemoveAt(i);
				}
				
				UE_LOG(LogTemp, Log, TEXT("Removed %d %s from inventory"), 
					Quantity, *ItemID.ToString());
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Not enough %s in inventory (has %d, needs %d)"), 
					*ItemID.ToString(), Inventory[i].Quantity, Quantity);
				return false;
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Item %s not found in inventory"), *ItemID.ToString());
	return false;
}

int32 UHydroGrowSaveGame::GetInventoryItemCount(FName ItemID) const
{
	for (const FInventoryItemData& Item : Inventory)
	{
		if (Item.ItemID == ItemID)
		{
			return Item.Quantity;
		}
	}
	return 0;
}

void UHydroGrowSaveGame::UnlockPlant(FName PlantID)
{
	if (!UnlockedPlants.Contains(PlantID))
	{
		UnlockedPlants.Add(PlantID);
		UE_LOG(LogTemp, Warning, TEXT("Unlocked plant: %s"), *PlantID.ToString());
	}
}

void UHydroGrowSaveGame::UnlockEquipment(FName EquipmentID)
{
	if (!UnlockedEquipment.Contains(EquipmentID))
	{
		UnlockedEquipment.Add(EquipmentID);
		UE_LOG(LogTemp, Warning, TEXT("Unlocked equipment: %s"), *EquipmentID.ToString());
	}
}

void UHydroGrowSaveGame::UnlockAchievement(const FString& AchievementID)
{
	if (!UnlockedAchievements.Contains(AchievementID))
	{
		UnlockedAchievements.Add(AchievementID);
		LifetimeStats.UnlockedAchievements.Add(AchievementID);
		UE_LOG(LogTemp, Warning, TEXT("Unlocked achievement: %s"), *AchievementID);
	}
}

bool UHydroGrowSaveGame::IsPlantUnlocked(FName PlantID) const
{
	return UnlockedPlants.Contains(PlantID);
}

bool UHydroGrowSaveGame::IsEquipmentUnlocked(FName EquipmentID) const
{
	return UnlockedEquipment.Contains(EquipmentID);
}

bool UHydroGrowSaveGame::IsAchievementUnlocked(const FString& AchievementID) const
{
	return UnlockedAchievements.Contains(AchievementID);
}