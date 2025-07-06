#include "Plants/PlantMeshConfigurator.h"

FPlantMeshConfiguration UPlantMeshConfigurator::GetPlantConfiguration(FName PlantSpeciesID) const
{
	for (const FPlantMeshConfiguration& Config : PlantConfigurations)
	{
		if (Config.PlantSpeciesID == PlantSpeciesID)
		{
			return Config;
		}
	}

	// Return empty configuration if not found
	return FPlantMeshConfiguration();
}

TArray<FName> UPlantMeshConfigurator::GetAvailablePlantSpecies() const
{
	TArray<FName> SpeciesIDs;
	for (const FPlantMeshConfiguration& Config : PlantConfigurations)
	{
		SpeciesIDs.Add(Config.PlantSpeciesID);
	}
	return SpeciesIDs;
}

void UPlantMeshConfigurator::CreatePresetConfigurations()
{
	PlantConfigurations.Empty();

	// Add common Ultimate Farming Kit plants
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Tomato"), TEXT("Tomato Plant"), TEXT("Tomatoes")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Lettuce"), TEXT("Lettuce"), TEXT("Lettuce")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Carrot"), TEXT("Carrot"), TEXT("Carrot")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Strawberry"), TEXT("Strawberry Plant"), TEXT("Strawberry")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Eggplant"), TEXT("Eggplant"), TEXT("Eggplant")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Cucumber"), TEXT("Cucumber Plant"), TEXT("Cucumber")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Pepper"), TEXT("Pepper Plant"), TEXT("Pepper")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Cabbage"), TEXT("Cabbage"), TEXT("Cabbage")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Broccoli"), TEXT("Broccoli"), TEXT("Broccoli")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Spinach"), TEXT("Spinach"), TEXT("Spinach")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Potato"), TEXT("Potato Plant"), TEXT("Potato")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Onion"), TEXT("Onion"), TEXT("Onion")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Garlic"), TEXT("Garlic"), TEXT("Garlic")));
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Basil"), TEXT("Basil"), TEXT("Mint"))); // Using mint mesh for herbs
	PlantConfigurations.Add(CreatePlantConfig(TEXT("Arugula"), TEXT("Arugula"), TEXT("Arugula")));
}

FPlantMeshConfiguration UPlantMeshConfigurator::CreatePlantConfig(
	const FName& SpeciesID,
	const FString& DisplayName,
	const FString& MeshBaseName
) const
{
	FPlantMeshConfiguration Config;
	Config.PlantSpeciesID = SpeciesID;
	Config.PlantDisplayName = DisplayName;

	// Construct mesh paths based on Ultimate Farming Kit naming convention
	FString BasePath = TEXT("/Game/UltimateFarming/Meshes/SM_");

	Config.StarterMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(BasePath + MeshBaseName + TEXT("_Starter")));
	Config.StageAMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(BasePath + MeshBaseName + TEXT("_A")));
	Config.StageBMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(BasePath + MeshBaseName + TEXT("_B")));
	Config.StageCMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(BasePath + MeshBaseName + TEXT("_C")));
	
	// Some plants have flower meshes
	Config.FlowerMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(BasePath + MeshBaseName + TEXT("_A_Flower")));
	
	// Some plants have harvested meshes
	Config.HarvestedMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(BasePath + MeshBaseName + TEXT("_A_Harvested")));

	return Config;
}