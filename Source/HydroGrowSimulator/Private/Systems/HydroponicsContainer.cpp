#include "Systems/HydroponicsContainer.h"
#include "Plants/PlantActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Network/HydroGrowNetworkGameMode.h"
#include "GameFramework/GameModeBase.h"

// Network serialization for FPlantSlot
bool FPlantSlot::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << bIsOccupied;
	Ar << PlantActor;
	Ar << SlotLocation;
	Ar << SlotIndex;
	Ar << PlantedByPlayerID;
	
	bOutSuccess = true;
	return true;
}

AHydroponicsContainer::AHydroponicsContainer()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(10.0f);

	// Create components
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	ContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	ContainerMesh->SetupAttachment(RootComponent);

	WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterMesh"));
	WaterMesh->SetupAttachment(RootComponent);

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Initialize default values
	ContainerType = EContainerType::DWC;
	bPumpRunning = false;
	WaterFlowRate = 1.0f;
	EnergyConsumptionRate = 0.0f;
	
	// Network defaults
	OwnerPlayerID = TEXT("");
	bIsSharedContainer = false;

	// Configuration defaults
	BaseEnergyConsumption = 10.0f;
	PumpEnergyConsumption = 25.0f;
	WaterCapacity = 100.0f;

	// Set up type efficiency multipliers
	TypeEfficiencyMultipliers.Add(EContainerType::DWC, 1.0f);
	TypeEfficiencyMultipliers.Add(EContainerType::EbbFlow, 1.1f);
	TypeEfficiencyMultipliers.Add(EContainerType::NFT, 1.2f);
	TypeEfficiencyMultipliers.Add(EContainerType::Aeroponics, 1.5f);
	TypeEfficiencyMultipliers.Add(EContainerType::DripSystem, 0.9f);

	// Initialize environmental conditions
	CurrentConditions.PHLevel = 6.0f;
	CurrentConditions.ECLevel = 1.5f;
	CurrentConditions.Temperature = 21.0f;
	CurrentConditions.Humidity = 60.0f;
	CurrentConditions.LightIntensity = 1.0f;
	CurrentConditions.OxygenLevel = 1.0f;
	CurrentConditions.WaterLevel = 1.0f;

	// Initialize nutrient solution
	NutrientSolution.Nitrogen = 1.0f;
	NutrientSolution.Phosphorus = 1.0f;
	NutrientSolution.Potassium = 1.0f;
	NutrientSolution.Calcium = 1.0f;
	NutrientSolution.Magnesium = 1.0f;
	NutrientSolution.Sulfur = 1.0f;
	NutrientSolution.Iron = 1.0f;
	NutrientSolution.Manganese = 1.0f;
	NutrientSolution.Zinc = 1.0f;
}

void AHydroponicsContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, ContainerType, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, PlantSlots, COND_None);
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, CurrentConditions, COND_None);
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, NutrientSolution, COND_None);
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, bPumpRunning, COND_None);
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, OwnerPlayerID, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AHydroponicsContainer, bIsSharedContainer, COND_None);
}

bool AHydroponicsContainer::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	// Always relevant for all players in multiplayer sessions
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

void AHydroponicsContainer::BeginPlay()
{
	Super::BeginPlay();
	
	// Start with pump running for most container types (server only)
	if (HasAuthority() && ContainerType != EContainerType::DWC)
	{
		StartWaterPump();
	}
}

void AHydroponicsContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Only simulate on server
	if (HasAuthority())
	{
		UpdateEnvironmentalConditions(DeltaTime);
		UpdateWaterSystem(DeltaTime);
		UpdatePlantConditions();
	}
	
	// Visual effects on all clients
	UpdateVisualEffects();
}

void AHydroponicsContainer::InitializeContainer(EContainerType Type, int32 PlantCapacity)
{
	ContainerType = Type;
	CreatePlantSlots(PlantCapacity);
	
	// Adjust environmental conditions based on container type
	switch (ContainerType)
	{
	case EContainerType::DWC:
		CurrentConditions.OxygenLevel = 1.2f; // High oxygen from air stones
		EnergyConsumptionRate = BaseEnergyConsumption;
		break;
		
	case EContainerType::EbbFlow:
		WaterFlowRate = 0.5f; // Intermittent flooding
		EnergyConsumptionRate = BaseEnergyConsumption + PumpEnergyConsumption * 0.3f;
		break;
		
	case EContainerType::NFT:
		WaterFlowRate = 0.8f; // Constant thin film
		EnergyConsumptionRate = BaseEnergyConsumption + PumpEnergyConsumption * 0.5f;
		break;
		
	case EContainerType::Aeroponics:
		CurrentConditions.OxygenLevel = 1.5f; // Maximum oxygen exposure
		EnergyConsumptionRate = BaseEnergyConsumption + PumpEnergyConsumption * 0.8f;
		break;
		
	case EContainerType::DripSystem:
		WaterFlowRate = 0.3f; // Slow drip
		EnergyConsumptionRate = BaseEnergyConsumption + PumpEnergyConsumption * 0.2f;
		break;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Initialized %s container with %d slots"), 
		*UEnum::GetValueAsString(ContainerType), PlantCapacity);
}

bool AHydroponicsContainer::CanPlantSeed(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= PlantSlots.Num())
	{
		return false;
	}
	
	return !PlantSlots[SlotIndex].bIsOccupied;
}

bool AHydroponicsContainer::PlantSeed(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID)
{
	// Check permissions first
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::PlantSeeds))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to plant seeds"), *PlayerID);
		return false;
	}
	
	if (HasAuthority())
	{
		Server_PlantSeed(PlantSpeciesID, SlotIndex, PlayerID);
		return true;
	}
	else
	{
		Server_PlantSeed(PlantSpeciesID, SlotIndex, PlayerID);
		return true; // Assume success on client
	}
}

void AHydroponicsContainer::Server_PlantSeed_Implementation(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID)
{
	if (!CanPlantSeed(SlotIndex))
	{
		return;
	}
	
	// Spawn plant actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	FVector SpawnLocation = GetActorLocation() + PlantSlots[SlotIndex].SlotLocation;
	APlantActor* NewPlant = GetWorld()->SpawnActor<APlantActor>(APlantActor::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	
	if (NewPlant)
	{
		NewPlant->InitializePlant(PlantSpeciesID, this);
		
		PlantSlots[SlotIndex].bIsOccupied = true;
		PlantSlots[SlotIndex].PlantActor = NewPlant;
		PlantSlots[SlotIndex].PlantedByPlayerID = PlayerID;
		
		OnPlantAdded.Broadcast(NewPlant);
		OnContainerInteraction.Broadcast(PlayerID, FString::Printf(TEXT("Planted %s"), *PlantSpeciesID.ToString()));
		
		UE_LOG(LogTemp, Warning, TEXT("Player %s planted %s in slot %d"), *PlayerID, *PlantSpeciesID.ToString(), SlotIndex);
	}
}

bool AHydroponicsContainer::Server_PlantSeed_Validate(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID)
{
	return true;
}

bool AHydroponicsContainer::RemovePlant(int32 SlotIndex, const FString& PlayerID)
{
	// Check permissions
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::HarvestPlants))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to remove plants"), *PlayerID);
		return false;
	}
	
	if (HasAuthority())
	{
		Server_RemovePlant(SlotIndex, PlayerID);
		return true;
	}
	else
	{
		Server_RemovePlant(SlotIndex, PlayerID);
		return true;
	}
}

void AHydroponicsContainer::Server_RemovePlant_Implementation(int32 SlotIndex, const FString& PlayerID)
{
	if (SlotIndex < 0 || SlotIndex >= PlantSlots.Num() || !PlantSlots[SlotIndex].bIsOccupied)
	{
		return;
	}
	
	if (PlantSlots[SlotIndex].PlantActor)
	{
		PlantSlots[SlotIndex].PlantActor->Destroy();
	}
	
	PlantSlots[SlotIndex].bIsOccupied = false;
	PlantSlots[SlotIndex].PlantActor = nullptr;
	PlantSlots[SlotIndex].PlantedByPlayerID = TEXT("");
	
	OnPlantRemoved.Broadcast(SlotIndex);
	OnContainerInteraction.Broadcast(PlayerID, TEXT("Removed plant"));
}

bool AHydroponicsContainer::Server_RemovePlant_Validate(int32 SlotIndex, const FString& PlayerID)
{
	return true;
}

int32 AHydroponicsContainer::GetAvailableSlot() const
{
	for (int32 i = 0; i < PlantSlots.Num(); i++)
	{
		if (!PlantSlots[i].bIsOccupied)
		{
			return i;
		}
	}
	return -1;
}

void AHydroponicsContainer::SetPHLevel(float NewPH, const FString& PlayerID)
{
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::AdjustEnvironment))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to adjust pH"), *PlayerID);
		return;
	}
	
	if (HasAuthority())
	{
		Server_SetPHLevel(NewPH, PlayerID);
	}
	else
	{
		Server_SetPHLevel(NewPH, PlayerID);
	}
}

void AHydroponicsContainer::Server_SetPHLevel_Implementation(float NewPH, const FString& PlayerID)
{
	CurrentConditions.PHLevel = FMath::Clamp(NewPH, 4.0f, 8.0f);
	OnEnvironmentalChange.Broadcast(TEXT("pH"), CurrentConditions.PHLevel);
	OnContainerInteraction.Broadcast(PlayerID, FString::Printf(TEXT("Adjusted pH to %.2f"), CurrentConditions.PHLevel));
	
	UE_LOG(LogTemp, Log, TEXT("Player %s adjusted pH to %.2f"), *PlayerID, CurrentConditions.PHLevel);
}

bool AHydroponicsContainer::Server_SetPHLevel_Validate(float NewPH, const FString& PlayerID)
{
	return NewPH >= 4.0f && NewPH <= 8.0f;
}

void AHydroponicsContainer::SetECLevel(float NewEC, const FString& PlayerID)
{
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::AdjustEnvironment))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to adjust EC"), *PlayerID);
		return;
	}
	
	if (HasAuthority())
	{
		Server_SetECLevel(NewEC, PlayerID);
	}
	else
	{
		Server_SetECLevel(NewEC, PlayerID);
	}
}

void AHydroponicsContainer::Server_SetECLevel_Implementation(float NewEC, const FString& PlayerID)
{
	CurrentConditions.ECLevel = FMath::Clamp(NewEC, 0.0f, 4.0f);
	OnEnvironmentalChange.Broadcast(TEXT("EC"), CurrentConditions.ECLevel);
	OnContainerInteraction.Broadcast(PlayerID, FString::Printf(TEXT("Adjusted EC to %.2f"), CurrentConditions.ECLevel));
	
	UE_LOG(LogTemp, Log, TEXT("Player %s adjusted EC to %.2f"), *PlayerID, CurrentConditions.ECLevel);
}

bool AHydroponicsContainer::Server_SetECLevel_Validate(float NewEC, const FString& PlayerID)
{
	return NewEC >= 0.0f && NewEC <= 4.0f;
}

void AHydroponicsContainer::SetWaterLevel(float NewLevel)
{
	CurrentConditions.WaterLevel = FMath::Clamp(NewLevel, 0.0f, 1.0f);
	OnEnvironmentalChange.Broadcast(TEXT("Water Level"), CurrentConditions.WaterLevel);
}

void AHydroponicsContainer::AddNutrients(const FNutrientLevels& Nutrients, const FString& PlayerID)
{
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::AddNutrients))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to add nutrients"), *PlayerID);
		return;
	}
	
	if (HasAuthority())
	{
		Server_AddNutrients(Nutrients, PlayerID);
	}
	else
	{
		Server_AddNutrients(Nutrients, PlayerID);
	}
}

void AHydroponicsContainer::Server_AddNutrients_Implementation(const FNutrientLevels& Nutrients, const FString& PlayerID)
{
	// Add nutrients to the solution
	NutrientSolution.Nitrogen += Nutrients.Nitrogen;
	NutrientSolution.Phosphorus += Nutrients.Phosphorus;
	NutrientSolution.Potassium += Nutrients.Potassium;
	NutrientSolution.Calcium += Nutrients.Calcium;
	NutrientSolution.Magnesium += Nutrients.Magnesium;
	NutrientSolution.Sulfur += Nutrients.Sulfur;
	NutrientSolution.Iron += Nutrients.Iron;
	NutrientSolution.Manganese += Nutrients.Manganese;
	NutrientSolution.Zinc += Nutrients.Zinc;
	
	// Clamp values to reasonable ranges
	NutrientSolution.Nitrogen = FMath::Clamp(NutrientSolution.Nitrogen, 0.0f, 3.0f);
	NutrientSolution.Phosphorus = FMath::Clamp(NutrientSolution.Phosphorus, 0.0f, 3.0f);
	NutrientSolution.Potassium = FMath::Clamp(NutrientSolution.Potassium, 0.0f, 3.0f);
	
	// Update EC based on nutrient concentration
	float TotalNutrients = (NutrientSolution.Nitrogen + NutrientSolution.Phosphorus + NutrientSolution.Potassium) / 3.0f;
	CurrentConditions.ECLevel = TotalNutrients * 1.5f;
	
	OnContainerInteraction.Broadcast(PlayerID, TEXT("Added nutrients"));
	UE_LOG(LogTemp, Log, TEXT("Player %s added nutrients, new EC: %.2f"), *PlayerID, CurrentConditions.ECLevel);
}

bool AHydroponicsContainer::Server_AddNutrients_Validate(const FNutrientLevels& Nutrients, const FString& PlayerID)
{
	return true;
}

void AHydroponicsContainer::StartWaterPump(const FString& PlayerID)
{
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::OperateEquipment))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to operate equipment"), *PlayerID);
		return;
	}
	
	if (HasAuthority())
	{
		Server_StartWaterPump(PlayerID);
	}
	else
	{
		Server_StartWaterPump(PlayerID);
	}
}

void AHydroponicsContainer::StopWaterPump(const FString& PlayerID)
{
	if (!PlayerID.IsEmpty() && !CanPlayerInteract(PlayerID, EContainerPermission::OperateEquipment))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have permission to operate equipment"), *PlayerID);
		return;
	}
	
	if (HasAuthority())
	{
		Server_StopWaterPump(PlayerID);
	}
	else
	{
		Server_StopWaterPump(PlayerID);
	}
}

void AHydroponicsContainer::Server_StartWaterPump_Implementation(const FString& PlayerID)
{
	if (ContainerType == EContainerType::DWC)
	{
		UE_LOG(LogTemp, Warning, TEXT("DWC containers don't use water pumps"));
		return;
	}
	
	bPumpRunning = true;
	EnergyConsumptionRate = BaseEnergyConsumption + PumpEnergyConsumption;
	
	OnContainerInteraction.Broadcast(PlayerID, TEXT("Started water pump"));
	UE_LOG(LogTemp, Log, TEXT("Player %s started water pump"), *PlayerID);
}

bool AHydroponicsContainer::Server_StartWaterPump_Validate(const FString& PlayerID)
{
	return true;
}

void AHydroponicsContainer::Server_StopWaterPump_Implementation(const FString& PlayerID)
{
	bPumpRunning = false;
	EnergyConsumptionRate = BaseEnergyConsumption;
	
	OnContainerInteraction.Broadcast(PlayerID, TEXT("Stopped water pump"));
	UE_LOG(LogTemp, Log, TEXT("Player %s stopped water pump"), *PlayerID);
}

bool AHydroponicsContainer::Server_StopWaterPump_Validate(const FString& PlayerID)
{
	return true;
}

int32 AHydroponicsContainer::GetPlantCount() const
{
	int32 Count = 0;
	for (const FPlantSlot& Slot : PlantSlots)
	{
		if (Slot.bIsOccupied)
		{
			Count++;
		}
	}
	return Count;
}

float AHydroponicsContainer::GetEnergyConsumption() const
{
	return EnergyConsumptionRate;
}

TArray<APlantActor*> AHydroponicsContainer::GetAllPlants() const
{
	TArray<APlantActor*> Plants;
	for (const FPlantSlot& Slot : PlantSlots)
	{
		if (Slot.bIsOccupied && Slot.PlantActor)
		{
			Plants.Add(Slot.PlantActor);
		}
	}
	return Plants;
}

void AHydroponicsContainer::UpdateEnvironmentalConditions(float DeltaTime)
{
	SimulatePHDrift(DeltaTime);
	SimulateNutrientDepletion(DeltaTime);
	SimulateWaterEvaporation(DeltaTime);
}

void AHydroponicsContainer::UpdateWaterSystem(float DeltaTime)
{
	if (bPumpRunning)
	{
		// Pump maintains water circulation and oxygenation
		CurrentConditions.OxygenLevel = FMath::Min(CurrentConditions.OxygenLevel + 0.1f * DeltaTime, 1.5f);
		
		// Better nutrient distribution with pump running
		UpdateNutrientDistribution();
	}
	else
	{
		// Oxygen levels drop without circulation
		CurrentConditions.OxygenLevel = FMath::Max(CurrentConditions.OxygenLevel - 0.05f * DeltaTime, 0.3f);
	}
}

void AHydroponicsContainer::UpdateNutrientDistribution()
{
	// Even distribution of nutrients when pump is running
	// This affects how plants receive nutrients
	
	// Apply container type efficiency
	const float* EfficiencyMultiplier = TypeEfficiencyMultipliers.Find(ContainerType);
	if (EfficiencyMultiplier)
	{
		// Apply efficiency to nutrient availability
		// This would be used in plant calculations
	}
}

void AHydroponicsContainer::UpdatePlantConditions()
{
	// Update all plants with current environmental conditions
	for (const FPlantSlot& Slot : PlantSlots)
	{
		if (Slot.bIsOccupied && Slot.PlantActor)
		{
			Slot.PlantActor->SetEnvironmentalConditions(CurrentConditions);
			Slot.PlantActor->ApplyNutrients(NutrientSolution);
		}
	}
}

void AHydroponicsContainer::CreatePlantSlots(int32 Capacity)
{
	PlantSlots.Empty();
	PlantSlots.SetNum(Capacity);
	
	// Arrange slots in a grid pattern
	int32 SlotsPerRow = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(Capacity)));
	float SlotSpacing = 30.0f; // cm between slots
	
	for (int32 i = 0; i < Capacity; i++)
	{
		int32 Row = i / SlotsPerRow;
		int32 Col = i % SlotsPerRow;
		
		FVector SlotLocation;
		SlotLocation.X = (Col - SlotsPerRow / 2.0f) * SlotSpacing;
		SlotLocation.Y = (Row - Capacity / SlotsPerRow / 2.0f) * SlotSpacing;
		SlotLocation.Z = 0.0f;
		
		PlantSlots[i].SlotLocation = SlotLocation;
		PlantSlots[i].SlotIndex = i;
		PlantSlots[i].bIsOccupied = false;
		PlantSlots[i].PlantActor = nullptr;
	}
}

void AHydroponicsContainer::UpdateVisualEffects()
{
	// Update water mesh visibility and scale based on water level
	if (WaterMesh)
	{
		FVector WaterScale = FVector(1.0f, 1.0f, CurrentConditions.WaterLevel);
		WaterMesh->SetRelativeScale3D(WaterScale);
	}
	
	// Visual indicators for system status could be added here
}

void AHydroponicsContainer::SimulatePHDrift(float DeltaTime)
{
	// pH naturally drifts over time based on plant uptake and system type
	float DriftRate = 0.1f; // pH units per day
	float DriftDirection = FMath::RandRange(-1.0f, 1.0f);
	
	// Plants affect pH through nutrient uptake
	float PlantEffect = GetPlantCount() * 0.02f * DeltaTime;
	
	CurrentConditions.PHLevel += (DriftDirection * DriftRate + PlantEffect) * DeltaTime / 86400.0f;
	CurrentConditions.PHLevel = FMath::Clamp(CurrentConditions.PHLevel, 4.0f, 8.0f);
}

void AHydroponicsContainer::SimulateNutrientDepletion(float DeltaTime)
{
	// Plants consume nutrients over time
	float ConsumptionRate = GetPlantCount() * 0.01f * DeltaTime;
	
	NutrientSolution.Nitrogen = FMath::Max(NutrientSolution.Nitrogen - ConsumptionRate, 0.0f);
	NutrientSolution.Phosphorus = FMath::Max(NutrientSolution.Phosphorus - ConsumptionRate * 0.8f, 0.0f);
	NutrientSolution.Potassium = FMath::Max(NutrientSolution.Potassium - ConsumptionRate * 1.2f, 0.0f);
	
	// Update EC based on remaining nutrients
	float TotalNutrients = (NutrientSolution.Nitrogen + NutrientSolution.Phosphorus + NutrientSolution.Potassium) / 3.0f;
	CurrentConditions.ECLevel = TotalNutrients * 1.5f;
}

void AHydroponicsContainer::SimulateWaterEvaporation(float DeltaTime)
{
	// Water evaporates slowly over time
	float EvaporationRate = 0.05f * DeltaTime / 86400.0f; // 5% per day
	
	CurrentConditions.WaterLevel = FMath::Max(CurrentConditions.WaterLevel - EvaporationRate, 0.0f);
	
	// Low water levels affect other conditions
	if (CurrentConditions.WaterLevel < 0.3f)
	{
		// Concentrated nutrients
		CurrentConditions.ECLevel *= 1.1f;
		
		// Reduced oxygen
		CurrentConditions.OxygenLevel *= 0.9f;
	}
}

// Network permission functions
bool AHydroponicsContainer::CanPlayerInteract(const FString& PlayerID, EContainerPermission Permission) const
{
	if (PlayerID.IsEmpty())
	{
		return true; // Allow local/single player interactions
	}
	
	// Container owner can always interact
	if (OwnerPlayerID == PlayerID)
	{
		return true;
	}
	
	// Check shared access
	if (!bIsSharedContainer)
	{
		return false;
	}
	
	// Use the game mode's permission system
	return HasPermission(PlayerID, Permission);
}

void AHydroponicsContainer::SetContainerOwner(const FString& PlayerID)
{
	if (HasAuthority())
	{
		OwnerPlayerID = PlayerID;
	}
}

void AHydroponicsContainer::SetSharedAccess(bool bShared)
{
	if (HasAuthority())
	{
		bIsSharedContainer = bShared;
	}
}

class AHydroGrowNetworkGameMode* AHydroponicsContainer::GetNetworkGameMode() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<AHydroGrowNetworkGameMode>(World->GetAuthGameMode());
	}
	return nullptr;
}

bool AHydroponicsContainer::HasPermission(const FString& PlayerID, EContainerPermission Permission) const
{
	if (AHydroGrowNetworkGameMode* GameMode = GetNetworkGameMode())
	{
		switch (Permission)
		{
		case EContainerPermission::PlantSeeds:
			return GameMode->HasPlayerPermission(PlayerID, TEXT("bCanPlantSeeds"));
		case EContainerPermission::HarvestPlants:
			return GameMode->HasPlayerPermission(PlayerID, TEXT("bCanHarvestPlants"));
		case EContainerPermission::AdjustEnvironment:
			return GameMode->HasPlayerPermission(PlayerID, TEXT("bCanAdjustEnvironment"));
		case EContainerPermission::AddNutrients:
			return GameMode->HasPlayerPermission(PlayerID, TEXT("bCanAddNutrients"));
		case EContainerPermission::OperateEquipment:
			return GameMode->HasPlayerPermission(PlayerID, TEXT("bCanOperateEquipment"));
		default:
			return false;
		}
	}
	return true; // Default to allowing if no game mode
}