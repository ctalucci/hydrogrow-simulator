#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StaticMesh.h"
#include "PlantMeshConfigurator.generated.h"

/**
 * Helper class for configuring Ultimate Farming Kit plant meshes
 */
USTRUCT(BlueprintType)
struct HYDROGROWSIMULATOR_API FPlantMeshConfiguration
{
	GENERATED_BODY()

	// Plant species identifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Info")
	FName PlantSpeciesID;

	// Display name for the plant
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Info")
	FString PlantDisplayName;

	// Mesh for starter/seed stage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> StarterMesh;

	// Mesh for stage A (young plant)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> StageAMesh;

	// Mesh for stage B (medium plant)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> StageBMesh;

	// Mesh for stage C (mature plant)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> StageCMesh;

	// Mesh for flowering stage (if available)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> FlowerMesh;

	// Mesh for harvested state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> HarvestedMesh;

	// Optional dead/wilted mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Stage Meshes")
	TSoftObjectPtr<UStaticMesh> DeadMesh;

	FPlantMeshConfiguration()
	{
		PlantSpeciesID = NAME_None;
		PlantDisplayName = TEXT("Unknown Plant");
	}
};

/**
 * Utility class for managing Ultimate Farming Kit plant mesh configurations
 */
UCLASS(BlueprintType, Blueprintable)
class HYDROGROWSIMULATOR_API UPlantMeshConfigurator : public UObject
{
	GENERATED_BODY()

public:
	// Predefined configurations for Ultimate Farming Kit plants
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Configurations")
	TArray<FPlantMeshConfiguration> PlantConfigurations;

	// Get configuration for a specific plant species
	UFUNCTION(BlueprintCallable, Category = "Plant Mesh")
	FPlantMeshConfiguration GetPlantConfiguration(FName PlantSpeciesID) const;

	// Get all available plant species IDs
	UFUNCTION(BlueprintCallable, Category = "Plant Mesh")
	TArray<FName> GetAvailablePlantSpecies() const;

	// Create preset configurations for common Ultimate Farming Kit plants
	UFUNCTION(CallInEditor = true, Category = "Plant Mesh")
	void CreatePresetConfigurations();

protected:
	// Helper function to create a plant configuration
	FPlantMeshConfiguration CreatePlantConfig(
		const FName& SpeciesID,
		const FString& DisplayName,
		const FString& MeshBaseName
	) const;
};