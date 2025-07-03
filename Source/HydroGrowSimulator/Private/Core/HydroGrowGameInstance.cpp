#include "Core/HydroGrowGameInstance.h"
#include "Core/HydroGrowSaveGame.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

UHydroGrowGameInstance::UHydroGrowGameInstance()
{
	GraphicsQualityLevel = 2; // Medium quality by default
}

void UHydroGrowGameInstance::Init()
{
	Super::Init();
	
	InitializeDefaultSettings();
	LoadSettings();
	
	// Load data tables asynchronously
	bool bDataLoadSuccess = true;
	
	if (!PlantDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plant Data Table not assigned!"));
		bDataLoadSuccess = false;
	}
	
	if (!EquipmentDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment Data Table not assigned!"));
		bDataLoadSuccess = false;
	}
	
	OnDataLoaded.Broadcast(bDataLoadSuccess);
}

const FPlantSpeciesData* UHydroGrowGameInstance::GetPlantData(FName PlantID) const
{
	if (!PlantDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Plant Data Table is null!"));
		return nullptr;
	}
	
	return PlantDataTable->FindRow<FPlantSpeciesData>(PlantID, TEXT("PlantData"));
}

const FEquipmentData* UHydroGrowGameInstance::GetEquipmentData(FName EquipmentID) const
{
	if (!EquipmentDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Equipment Data Table is null!"));
		return nullptr;
	}
	
	return EquipmentDataTable->FindRow<FEquipmentData>(EquipmentID, TEXT("EquipmentData"));
}

FPlantSpeciesData UHydroGrowGameInstance::GetPlantDataCopy(FName PlantID) const
{
	const FPlantSpeciesData* PlantData = GetPlantData(PlantID);
	if (PlantData)
	{
		return *PlantData;
	}
	return FPlantSpeciesData();
}

FEquipmentData UHydroGrowGameInstance::GetEquipmentDataCopy(FName EquipmentID) const
{
	const FEquipmentData* EquipmentData = GetEquipmentData(EquipmentID);
	if (EquipmentData)
	{
		return *EquipmentData;
	}
	return FEquipmentData();
}

TArray<FName> UHydroGrowGameInstance::GetUnlockedPlants(int32 PlayerLevel) const
{
	TArray<FName> UnlockedPlants;
	
	if (!PlantDataTable)
	{
		return UnlockedPlants;
	}
	
	TArray<FName> RowNames = PlantDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FPlantSpeciesData* PlantData = GetPlantData(RowName);
		if (PlantData && PlantData->UnlockLevel <= PlayerLevel)
		{
			UnlockedPlants.Add(RowName);
		}
	}
	
	return UnlockedPlants;
}

TArray<FName> UHydroGrowGameInstance::GetUnlockedEquipment(int32 PlayerLevel) const
{
	TArray<FName> UnlockedEquipment;
	
	if (!EquipmentDataTable)
	{
		return UnlockedEquipment;
	}
	
	TArray<FName> RowNames = EquipmentDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FEquipmentData* EquipmentData = GetEquipmentData(RowName);
		if (EquipmentData && EquipmentData->UnlockLevel <= PlayerLevel)
		{
			UnlockedEquipment.Add(RowName);
		}
	}
	
	return UnlockedEquipment;
}

bool UHydroGrowGameInstance::SaveGameData()
{
	if (!CurrentSaveGame)
	{
		CurrentSaveGame = Cast<UHydroGrowSaveGame>(UGameplayStatics::CreateSaveGameObject(UHydroGrowSaveGame::StaticClass()));
	}
	
	if (CurrentSaveGame)
	{
		// Save game data will be populated by GameMode
		bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, TEXT("HydroGrowSave"), 0);
		
		if (bSaveSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game saved successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save game"));
		}
		
		return bSaveSuccess;
	}
	
	return false;
}

bool UHydroGrowGameInstance::LoadGameData()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("HydroGrowSave"), 0))
	{
		CurrentSaveGame = Cast<UHydroGrowSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("HydroGrowSave"), 0));
		
		if (CurrentSaveGame)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game loaded successfully"));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No save game found"));
	}
	
	return false;
}

bool UHydroGrowGameInstance::HasSaveData() const
{
	return UGameplayStatics::DoesSaveGameExist(TEXT("HydroGrowSave"), 0);
}

void UHydroGrowGameInstance::SetGraphicsQuality(int32 QualityLevel)
{
	GraphicsQualityLevel = FMath::Clamp(QualityLevel, 0, 3);
	
	// Apply graphics settings
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->SetOverallScalabilityLevel(GraphicsQualityLevel);
		UserSettings->ApplySettings(false);
	}
	
	SaveSettings();
}

void UHydroGrowGameInstance::SetAudioVolume(const FString& AudioType, float Volume)
{
	float ClampedVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	AudioSettings.Add(AudioType, ClampedVolume);
	
	// Apply audio settings to sound classes
	// This would be implemented with actual sound class references
	
	SaveSettings();
}

float UHydroGrowGameInstance::GetAudioVolume(const FString& AudioType) const
{
	const float* FoundVolume = AudioSettings.Find(AudioType);
	return FoundVolume ? *FoundVolume : 1.0f;
}

void UHydroGrowGameInstance::InitializeDefaultSettings()
{
	// Default audio settings
	AudioSettings.Add(TEXT("Master"), 1.0f);
	AudioSettings.Add(TEXT("SFX"), 1.0f);
	AudioSettings.Add(TEXT("Music"), 0.7f);
	AudioSettings.Add(TEXT("Ambient"), 0.8f);
}

void UHydroGrowGameInstance::LoadSettings()
{
	// In a full implementation, this would load from a config file or registry
	// For now, we'll use defaults
	UE_LOG(LogTemp, Warning, TEXT("Loading settings (using defaults)"));
}

void UHydroGrowGameInstance::SaveSettings()
{
	// In a full implementation, this would save to a config file or registry
	UE_LOG(LogTemp, Warning, TEXT("Saving settings"));
}