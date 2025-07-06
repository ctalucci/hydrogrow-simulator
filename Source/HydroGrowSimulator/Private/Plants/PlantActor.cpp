#include "Plants/PlantActor.h"
#include "Plants/PlantMeshConfigurator.h"
#include "Core/HydroGrowGameInstance.h"
#include "Systems/HydroponicsContainer.h"
#include "Systems/TimeManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

APlantActor::APlantActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Enable replication
	bReplicates = true;
	bAlwaysRelevant = true;

	// Create components
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	PlantMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PlantMesh"));
	PlantMesh->SetupAttachment(RootComponent);

	// Create static mesh component for Ultimate Farming Kit meshes
	StaticPlantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticPlantMesh"));
	StaticPlantMesh->SetupAttachment(RootComponent);

	PotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotMesh"));
	PotMesh->SetupAttachment(RootComponent);

	// Initialize default values
	CurrentGrowthStage = EPlantGrowthStage::Seed;
	GrowthProgress = 0.0f;
	AgeInDays = 0.0f;
	MaxHealthPoints = 100.0f;
	HealthPoints = MaxHealthPoints;

	// Initialize effectiveness factors
	PHEffectiveness = 1.0f;
	NutrientEffectiveness = 1.0f;
	LightEffectiveness = 1.0f;
	TemperatureEffectiveness = 1.0f;
	OverallGrowthRate = 1.0f;

	// Default to not using static meshes
	bUseStaticMeshes = false;

	// Initially hide the static mesh
	StaticPlantMesh->SetVisibility(false);
}

void APlantActor::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = Cast<UHydroGrowGameInstance>(GetGameInstance());
	TimeManager = GetGameInstance()->GetSubsystem<UTimeManager>();
	
	// Initialize network tracking
	LastActionTime = FDateTime::Now();
	LastActionPlayer = TEXT("System");
	
	UpdateVisualAppearanceInternal();
}

void APlantActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate plant state to all clients
	DOREPLIFETIME(APlantActor, PlantSpeciesID);
	DOREPLIFETIME(APlantActor, CurrentGrowthStage);
	DOREPLIFETIME(APlantActor, GrowthProgress);
	DOREPLIFETIME(APlantActor, AgeInDays);
	DOREPLIFETIME(APlantActor, HealthPoints);
	DOREPLIFETIME(APlantActor, MaxHealthPoints);
	DOREPLIFETIME(APlantActor, LastActionPlayer);
	DOREPLIFETIME(APlantActor, LastActionTime);
}

void APlantActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsAlive())
	{
		UpdateGrowthProgress(DeltaTime);
		UpdateHealthPoints(DeltaTime);
		CalculateGrowthFactors();
		CheckForProblems();
	}
}

void APlantActor::InitializePlant(FName lantSpeciesID, AHydroponicsContainer* Container)
{
	this->PlantSpeciesID = PlantSpeciesID;
	this->ParentContainer = Container;
	
	const FPlantSpeciesData* PlantData = (GameInstance ? GameInstance->GetPlantData(PlantSpeciesID) : nullptr);
	if (PlantData)
	{
		MaxHealthPoints = 100.0f; // Base health
		HealthPoints = MaxHealthPoints;
		
		UE_LOG(LogTemp, Warning, TEXT("Initialized plant: %s"), *PlantData->DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find plant data for ID: %s"), *PlantSpeciesID.ToString());
	}
	
	UpdateVisualAppearanceInternal();
}

bool APlantActor::CanBeHarvested() const
{
	return CurrentGrowthStage == EPlantGrowthStage::Harvest && IsAlive();
}

bool APlantActor::CanHarvestPlant() const
{
	return CanBeHarvested();
}

int32 APlantActor::Harvest()
{
	if (!CanHarvestPlant())
	{
		return 0;
	}
	
	const FPlantSpeciesData* PlantData = (GameInstance ? GameInstance->GetPlantData(PlantSpeciesID) : nullptr);
	if (!PlantData)
	{
		return 0;
	}
	
	// Calculate yield based on health and growth conditions
	float YieldMultiplier = (HealthPoints / MaxHealthPoints) * OverallGrowthRate;
	YieldMultiplier = FMath::Clamp(YieldMultiplier, 0.1f, 1.5f); // Allow bonus yield for perfect conditions
	
	int32 FinalYield = FMath::RoundToInt(PlantData->BaseYield * YieldMultiplier);
	
	OnPlantHarvested.Broadcast(FinalYield);
	
	UE_LOG(LogTemp, Warning, TEXT("Harvested %s: %d yield (multiplier: %.2f)"), 
		*PlantData->DisplayName.ToString(), FinalYield, YieldMultiplier);
	
	// Reset plant or destroy it
	Destroy();
	
	return FinalYield;
}

int32 APlantActor::HarvestPlant(const FString& PlayerName)
{
	if (HasAuthority())
	{
		return Harvest();
	}
	else
	{
		// On client, send server request
		Server_HarvestPlant(PlayerName);
		return 0; // Return 0 for now, server will handle the actual harvest
	}
}

void APlantActor::WaterPlant(float WaterAmount)
{
	// Watering restores some health and helps with nutrient uptake
	float HealthRestore = WaterAmount * 5.0f;
	HealthPoints = FMath::Min(HealthPoints + HealthRestore, MaxHealthPoints);
	
	UE_LOG(LogTemp, Log, TEXT("Watered plant, health: %.1f"), HealthPoints);
}

void APlantActor::ApplyNutrients(const FNutrientLevels& Nutrients)
{
	CurrentNutrients = Nutrients;
	
	// Nutrients boost growth and health
	float NutrientBoost = (Nutrients.Nitrogen + Nutrients.Phosphorus + Nutrients.Potassium) / 3.0f;
	HealthPoints = FMath::Min(HealthPoints + NutrientBoost * 2.0f, MaxHealthPoints);
}

float APlantActor::GetGrowthPercentage() const
{
	return GrowthProgress * 100.0f;
}

float APlantActor::GetHealthPercentage() const
{
	return (HealthPoints / MaxHealthPoints) * 100.0f;
}

FPlantSpeciesData APlantActor::GetPlantData() const
{
	if (GameInstance)
	{
		const FPlantSpeciesData* PlantDataPtr = GameInstance->GetPlantData(PlantSpeciesID);
		if (PlantDataPtr)
		{
			return *PlantDataPtr;
		}
	}
	// Return default/empty plant data if not found
	return FPlantSpeciesData();
}

void APlantActor::SetEnvironmentalConditions(const FEnvironmentalConditions& Conditions)
{
	CurrentEnvironment = Conditions;
}

void APlantActor::UpdateGrowthProgress(float DeltaTime)
{
	const FPlantSpeciesData* PlantData = (GameInstance ? GameInstance->GetPlantData(PlantSpeciesID) : nullptr);
	if (!PlantData)
	{
		return;
	}
	
	// Convert real time to game time
	float GameDeltaTime = DeltaTime;
	if (TimeManager)
	{
		GameDeltaTime *= TimeManager->GetCurrentTimeScale();
	}
	
	// Update age
	AgeInDays += GameDeltaTime / 86400.0f; // Convert seconds to days
	
	// Calculate growth rate based on environmental factors
	float BaseGrowthRate = 1.0f / (PlantData->GrowthTimeInDays * 86400.0f); // Growth per second
	float ModifiedGrowthRate = BaseGrowthRate * OverallGrowthRate;
	
	// Apply growth
	GrowthProgress += ModifiedGrowthRate * GameDeltaTime;
	GrowthProgress = FMath::Clamp(GrowthProgress, 0.0f, 1.0f);
	
	// Check for stage transitions
	UpdateGrowthStage();
}

void APlantActor::UpdateGrowthStage()
{
	EPlantGrowthStage NewStage = CurrentGrowthStage;
	
	if (GrowthProgress >= HarvestThreshold)
	{
		NewStage = EPlantGrowthStage::Harvest;
	}
	else if (GrowthProgress >= FloweringThreshold)
	{
		NewStage = EPlantGrowthStage::Flowering;
	}
	else if (GrowthProgress >= VegetativeThreshold)
	{
		NewStage = EPlantGrowthStage::Vegetative;
	}
	else if (GrowthProgress >= SeedlingThreshold)
	{
		NewStage = EPlantGrowthStage::Seedling;
	}
	
	if (NewStage != CurrentGrowthStage)
	{
		CurrentGrowthStage = NewStage;
		OnGrowthStageChanged.Broadcast(CurrentGrowthStage);
		UpdateVisualAppearanceInternal();
		
		UE_LOG(LogTemp, Warning, TEXT("Plant growth stage changed to: %d"), (int32)CurrentGrowthStage);
	}
}

void APlantActor::UpdateHealthPoints(float DeltaTime)
{
	// Health degrades if environmental conditions are poor
	float HealthChange = 0.0f;
	
	// Poor growth conditions cause health loss
	if (OverallGrowthRate < 0.5f)
	{
		HealthChange -= (1.0f - OverallGrowthRate) * 10.0f * DeltaTime;
	}
	
	// Good conditions slowly restore health
	if (OverallGrowthRate > 0.8f)
	{
		HealthChange += (OverallGrowthRate - 0.8f) * 5.0f * DeltaTime;
	}
	
	HealthPoints = FMath::Clamp(HealthPoints + HealthChange, 0.0f, MaxHealthPoints);
	
	// Check if plant dies
	if (HealthPoints <= 0.0f)
	{
		CurrentGrowthStage = EPlantGrowthStage::Dead;
		UpdateVisualAppearanceInternal();
		UE_LOG(LogTemp, Warning, TEXT("Plant has died"));
	}
}

void APlantActor::CalculateGrowthFactors()
{
	const FPlantSpeciesData* PlantData = (GameInstance ? GameInstance->GetPlantData(PlantSpeciesID) : nullptr);
	if (!PlantData)
	{
		return;
	}
	
	// Calculate individual effectiveness factors
	PHEffectiveness = CalculatePHEffect(CurrentEnvironment.PHLevel, PlantData->OptimalPHRange);
	NutrientEffectiveness = CalculateNutrientEffect(CurrentEnvironment.ECLevel, PlantData->OptimalECRange);
	LightEffectiveness = CalculateLightEffect(CurrentEnvironment.LightIntensity, PlantData->LightHoursRequired);
	TemperatureEffectiveness = CalculateTemperatureEffect(CurrentEnvironment.Temperature, PlantData->OptimalTemperatureRange);
	
	// Overall growth rate is the product of all factors
	OverallGrowthRate = PHEffectiveness * NutrientEffectiveness * LightEffectiveness * TemperatureEffectiveness;
	OverallGrowthRate = FMath::Clamp(OverallGrowthRate, 0.0f, 2.0f);
}

void APlantActor::UpdateVisualAppearance()
{
	UpdateVisualAppearanceInternal();
}

void APlantActor::ApplyMeshConfiguration(const FPlantMeshConfiguration& Config)
{
	// Enable static mesh mode
	bUseStaticMeshes = true;

	// Clear existing mesh array
	GrowthStageMeshes.Empty();
	GrowthStageMeshes.SetNum(5); // Make room for 5 growth stages

	// Load and assign meshes
	if (Config.StarterMesh.IsValid())
	{
		GrowthStageMeshes[0] = Config.StarterMesh.LoadSynchronous();
	}
	if (Config.StageAMesh.IsValid())
	{
		GrowthStageMeshes[1] = Config.StageAMesh.LoadSynchronous();
	}
	if (Config.StageBMesh.IsValid())
	{
		GrowthStageMeshes[2] = Config.StageBMesh.LoadSynchronous();
	}
	if (Config.StageCMesh.IsValid())
	{
		GrowthStageMeshes[3] = Config.StageCMesh.LoadSynchronous();
	}
	if (Config.FlowerMesh.IsValid())
	{
		GrowthStageMeshes[4] = Config.FlowerMesh.LoadSynchronous();
	}

	// Assign special state meshes
	if (Config.HarvestedMesh.IsValid())
	{
		HarvestedMesh = Config.HarvestedMesh.LoadSynchronous();
	}
	if (Config.DeadMesh.IsValid())
	{
		DeadMesh = Config.DeadMesh.LoadSynchronous();
	}

	// Set the plant species ID if provided
	if (Config.PlantSpeciesID != NAME_None)
	{
		PlantSpeciesID = Config.PlantSpeciesID;
	}

	// Update visual appearance with new meshes
	UpdateVisualAppearance();

	UE_LOG(LogTemp, Log, TEXT("Applied mesh configuration for plant: %s"), *Config.PlantDisplayName);
}

void APlantActor::UpdateVisualAppearanceInternal()
{
	// Calculate visual effects
	FVector Scale = FVector::OneVector;
	
	// Scale based on growth progress
	float GrowthScale = FMath::Lerp(0.1f, 1.0f, GrowthProgress);
	Scale *= GrowthScale;
	
	// Reduce scale if unhealthy
	float HealthScale = FMath::Lerp(0.7f, 1.0f, HealthPoints / MaxHealthPoints);
	Scale *= HealthScale;
	
	if (bUseStaticMeshes && StaticPlantMesh)
	{
		// Use Ultimate Farming Kit static meshes
		PlantMesh->SetVisibility(false);
		StaticPlantMesh->SetVisibility(true);
		
		// Select appropriate mesh based on growth stage and health
		UStaticMesh* SelectedMesh = GetMeshForCurrentState();
		if (SelectedMesh && StaticPlantMesh->GetStaticMesh() != SelectedMesh)
		{
			StaticPlantMesh->SetStaticMesh(SelectedMesh);
		}
		
		StaticPlantMesh->SetWorldScale3D(Scale);
	}
	else if (PlantMesh)
	{
		// Use procedural mesh system
		StaticPlantMesh->SetVisibility(false);
		PlantMesh->SetVisibility(true);
		
		PlantMesh->SetWorldScale3D(Scale);
		
		// Change color based on health
		// This would be implemented with material parameter changes
	}
}

UStaticMesh* APlantActor::GetMeshForCurrentState() const
{
	// Handle special states first
	if (CurrentGrowthStage == EPlantGrowthStage::Dead && DeadMesh)
	{
		return DeadMesh;
	}
	
	if (CurrentGrowthStage == EPlantGrowthStage::Harvest && HarvestedMesh)
	{
		return HarvestedMesh;
	}
	
	// Map growth stages to mesh array indices
	int32 MeshIndex = 0;
	switch (CurrentGrowthStage)
	{
		case EPlantGrowthStage::Seed:
		case EPlantGrowthStage::Germination:
			MeshIndex = 0; // Starter mesh
			break;
		case EPlantGrowthStage::Seedling:
			MeshIndex = 1; // Small plant
			break;
		case EPlantGrowthStage::Vegetative:
			MeshIndex = 2; // Medium plant
			break;
		case EPlantGrowthStage::Flowering:
			MeshIndex = 3; // Large plant with flowers
			break;
		case EPlantGrowthStage::Harvest:
			MeshIndex = 4; // Fully grown with fruit
			break;
		default:
			MeshIndex = 0;
			break;
	}
	
	// Return mesh if available in array
	if (GrowthStageMeshes.IsValidIndex(MeshIndex) && GrowthStageMeshes[MeshIndex])
	{
		return GrowthStageMeshes[MeshIndex];
	}
	
	// Fallback to first available mesh
	if (GrowthStageMeshes.Num() > 0 && GrowthStageMeshes[0])
	{
		return GrowthStageMeshes[0];
	}
	
	return nullptr;
}

void APlantActor::CheckForProblems()
{
	// Log warnings for poor conditions
	if (PHEffectiveness < 0.7f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plant %s: Poor pH conditions (%.2f effectiveness)"), 
			*PlantSpeciesID.ToString(), PHEffectiveness);
	}
	
	if (NutrientEffectiveness < 0.7f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plant %s: Poor nutrient conditions (%.2f effectiveness)"), 
			*PlantSpeciesID.ToString(), NutrientEffectiveness);
	}
	
	if (LightEffectiveness < 0.7f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plant %s: Poor light conditions (%.2f effectiveness)"), 
			*PlantSpeciesID.ToString(), LightEffectiveness);
	}
	
	if (TemperatureEffectiveness < 0.7f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plant %s: Poor temperature conditions (%.2f effectiveness)"), 
			*PlantSpeciesID.ToString(), TemperatureEffectiveness);
	}
}

float APlantActor::CalculatePHEffect(float CurrentPH, const FVector2D& OptimalRange) const
{
	if (CurrentPH >= OptimalRange.X && CurrentPH <= OptimalRange.Y)
	{
		return 1.0f; // Perfect pH
	}
	
	// Calculate distance from optimal range
	float Distance = FMath::Min(FMath::Abs(CurrentPH - OptimalRange.X), FMath::Abs(CurrentPH - OptimalRange.Y));
	
	// Effectiveness drops exponentially with distance from optimal
	return FMath::Exp(-Distance * 2.0f);
}

float APlantActor::CalculateNutrientEffect(float CurrentEC, const FVector2D& OptimalRange) const
{
	if (CurrentEC >= OptimalRange.X && CurrentEC <= OptimalRange.Y)
	{
		return 1.0f; // Perfect EC
	}
	
	// Calculate distance from optimal range
	float Distance = FMath::Min(FMath::Abs(CurrentEC - OptimalRange.X), FMath::Abs(CurrentEC - OptimalRange.Y));
	
	// Effectiveness drops with distance from optimal
	return FMath::Max(0.1f, 1.0f - Distance * 0.5f);
}

float APlantActor::CalculateLightEffect(float LightIntensity, float RequiredHours) const
{
	// Simplified light calculation
	// In reality, this would account for daily light cycles
	
	float LightScore = LightIntensity * RequiredHours / 16.0f; // 16 hours as baseline
	return FMath::Clamp(LightScore, 0.1f, 1.2f);
}

float APlantActor::CalculateTemperatureEffect(float CurrentTemp, const FVector2D& OptimalRange) const
{
	if (CurrentTemp >= OptimalRange.X && CurrentTemp <= OptimalRange.Y)
	{
		return 1.0f; // Perfect temperature
	}
	
	// Calculate distance from optimal range
	float Distance = FMath::Min(FMath::Abs(CurrentTemp - OptimalRange.X), FMath::Abs(CurrentTemp - OptimalRange.Y));
	
	// Temperature has a strong effect on growth
	return FMath::Max(0.1f, 1.0f - Distance * 0.1f);
}

// Network function implementations
void APlantActor::Server_HarvestPlant_Implementation(const FString& PlayerName)
{
	if (CanHarvestPlant())
	{
		int32 Yield = Harvest();
		LastActionPlayer = PlayerName;
		LastActionTime = FDateTime::Now();
		
		// Broadcast to all clients
		Multicast_PlantHarvested(Yield, PlayerName);
	}
}

bool APlantActor::Server_HarvestPlant_Validate(const FString& PlayerName)
{
	return true; // Add any validation logic if needed
}

void APlantActor::Server_WaterPlant_Implementation(float WaterAmount, const FString& PlayerName)
{
	WaterPlant(WaterAmount);
	LastActionPlayer = PlayerName;
	LastActionTime = FDateTime::Now();
}

bool APlantActor::Server_WaterPlant_Validate(float WaterAmount, const FString& PlayerName)
{
	return WaterAmount > 0.0f; // Ensure positive water amount
}

void APlantActor::Server_ApplyNutrients_Implementation(const FNutrientLevels& Nutrients, const FString& PlayerName)
{
	ApplyNutrients(Nutrients);
	LastActionPlayer = PlayerName;
	LastActionTime = FDateTime::Now();
}
bool APlantActor::Server_ApplyNutrients_Validate(const FNutrientLevels& Nutrients, const FString& PlayerName)
{
	return Nutrients.IsValid(); // Ensure nutrient levels are valid
}

void APlantActor::Multicast_PlantGrowthStageChanged_Implementation(EPlantGrowthStage NewStage)
{
	UpdateVisualAppearanceInternal();
	OnGrowthStageChanged.Broadcast(NewStage);
}

void APlantActor::Multicast_PlantHarvested_Implementation(int32 Yield, const FString& PlayerName)
{
	OnPlantHarvested.Broadcast(Yield);
	UE_LOG(LogTemp, Warning, TEXT("Plant harvested by %s: %d yield"), *PlayerName, Yield);
}

void APlantActor::Multicast_PlantHealthChanged_Implementation(float NewHealth)
{
	UpdateVisualAppearanceInternal();
}

// Replication callbacks
void APlantActor::OnRep_GrowthStage()
{
	Multicast_PlantGrowthStageChanged(CurrentGrowthStage);
}

void APlantActor::OnRep_HealthPoints()
{
	Multicast_PlantHealthChanged(HealthPoints);
}

void APlantActor::OnRep_GrowthProgress()
{
	UpdateVisualAppearanceInternal();
}