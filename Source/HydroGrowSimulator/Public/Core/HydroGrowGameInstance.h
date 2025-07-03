#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HydroGrowTypes.h"
#include "HydroGrowGameInstance.generated.h"

class UDataTable;
class UHydroGrowSaveGame;

UCLASS()
class HYDROGROWSIMULATOR_API UHydroGrowGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UHydroGrowGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Data")
	const FPlantSpeciesData* GetPlantData(FName PlantID) const;

	UFUNCTION(BlueprintCallable, Category = "Data")
	const FEquipmentData* GetEquipmentData(FName EquipmentID) const;

	UFUNCTION(BlueprintCallable, Category = "Data")
	TArray<FName> GetUnlockedPlants(int32 PlayerLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Data")
	TArray<FName> GetUnlockedEquipment(int32 PlayerLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool SaveGameData();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool LoadGameData();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool HasSaveData() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetGraphicsQuality(int32 QualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetAudioVolume(const FString& AudioType, float Volume);

	UFUNCTION(BlueprintPure, Category = "Settings")
	float GetAudioVolume(const FString& AudioType) const;

protected:
	// Data Tables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* PlantDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	UDataTable* EquipmentDataTable;

	// Save Game
	UPROPERTY()
	UHydroGrowSaveGame* CurrentSaveGame;

	// Settings
	UPROPERTY()
	TMap<FString, float> AudioSettings;

	UPROPERTY()
	int32 GraphicsQualityLevel;

private:
	void InitializeDefaultSettings();
	void LoadSettings();
	void SaveSettings();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDataLoaded, bool, bSuccess);
	
	UPROPERTY(BlueprintAssignable)
	FOnDataLoaded OnDataLoaded;
};