#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Core/HydroGrowTypes.h"
#include "Network/HydroGrowNetworkTypes.h"
#include "HydroponicsContainer.generated.h"

class APlantActor;
class UStaticMeshComponent;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FPlantSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOccupied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlantActor* PlantActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SlotLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlantedByPlayerID;

	FPlantSlot()
	{
		bIsOccupied = false;
		PlantActor = nullptr;
		SlotLocation = FVector::ZeroVector;
		SlotIndex = -1;
		PlantedByPlayerID = TEXT("");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FPlantSlot> : public TStructOpsTypeTraitsBase2<FPlantSlot>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class HYDROGROWSIMULATOR_API AHydroponicsContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	AHydroponicsContainer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	void InitializeContainer(EContainerType Type, int32 PlantCapacity);

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool CanPlantSeed(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool PlantSeed(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID = TEXT(""));

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PlantSeed(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID);
	bool Server_PlantSeed_Validate(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID);
	void Server_PlantSeed_Implementation(FName PlantSpeciesID, int32 SlotIndex, const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "Container")
	bool RemovePlant(int32 SlotIndex, const FString& PlayerID = TEXT(""));

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemovePlant(int32 SlotIndex, const FString& PlayerID);
	bool Server_RemovePlant_Validate(int32 SlotIndex, const FString& PlayerID);
	void Server_RemovePlant_Implementation(int32 SlotIndex, const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "Container")
	int32 GetAvailableSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	void SetPHLevel(float NewPH, const FString& PlayerID = TEXT(""));

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPHLevel(float NewPH, const FString& PlayerID);
	bool Server_SetPHLevel_Validate(float NewPH, const FString& PlayerID);
	void Server_SetPHLevel_Implementation(float NewPH, const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void SetECLevel(float NewEC, const FString& PlayerID = TEXT(""));

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetECLevel(float NewEC, const FString& PlayerID);
	bool Server_SetECLevel_Validate(float NewEC, const FString& PlayerID);
	void Server_SetECLevel_Implementation(float NewEC, const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void SetWaterLevel(float NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void AddNutrients(const FNutrientLevels& Nutrients, const FString& PlayerID = TEXT(""));

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddNutrients(const FNutrientLevels& Nutrients, const FString& PlayerID);
	bool Server_AddNutrients_Validate(const FNutrientLevels& Nutrients, const FString& PlayerID);
	void Server_AddNutrients_Implementation(const FNutrientLevels& Nutrients, const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "Container")
	void StartWaterPump(const FString& PlayerID = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Container")
	void StopWaterPump(const FString& PlayerID = TEXT(""));

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartWaterPump(const FString& PlayerID);
	bool Server_StartWaterPump_Validate(const FString& PlayerID);
	void Server_StartWaterPump_Implementation(const FString& PlayerID);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopWaterPump(const FString& PlayerID);
	bool Server_StopWaterPump_Validate(const FString& PlayerID);
	void Server_StopWaterPump_Implementation(const FString& PlayerID);

	UFUNCTION(BlueprintPure, Category = "Container")
	EContainerType GetContainerType() const { return ContainerType; }

	UFUNCTION(BlueprintPure, Category = "Container")
	const FEnvironmentalConditions& GetEnvironmentalConditions() const { return CurrentConditions; }

	UFUNCTION(BlueprintPure, Category = "Container")
	int32 GetPlantCount() const;

	UFUNCTION(BlueprintPure, Category = "Container")
	int32 GetMaxCapacity() const { return PlantSlots.Num(); }

	UFUNCTION(BlueprintPure, Category = "Container")
	bool IsPumpRunning() const { return bPumpRunning; }

	UFUNCTION(BlueprintPure, Category = "Container")
	float GetEnergyConsumption() const;

	UFUNCTION(BlueprintCallable, Category = "Container")
	TArray<APlantActor*> GetAllPlants() const;

protected:
	// Core Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ContainerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WaterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionBox;

	// Container Configuration
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Container Setup")
	EContainerType ContainerType;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Container Setup")
	TArray<FPlantSlot> PlantSlots;

	// Environmental Conditions
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
	FEnvironmentalConditions CurrentConditions;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
	FNutrientLevels NutrientSolution;

	// System State
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "System")
	bool bPumpRunning;

	// Network State
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	FString OwnerPlayerID;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	bool bIsSharedContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
	float WaterFlowRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "System")
	float EnergyConsumptionRate;

	// Configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float BaseEnergyConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float PumpEnergyConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float WaterCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TMap<EContainerType, float> TypeEfficiencyMultipliers;

private:
	void UpdateEnvironmentalConditions(float DeltaTime);
	void UpdateWaterSystem(float DeltaTime);
	void UpdateNutrientDistribution();
	void UpdatePlantConditions();
	void CreatePlantSlots(int32 Capacity);
	void UpdateVisualEffects();

	// Environmental drift simulation
	void SimulatePHDrift(float DeltaTime);
	void SimulateNutrientDepletion(float DeltaTime);
	void SimulateWaterEvaporation(float DeltaTime);

public:
	// Permission checking
	UFUNCTION(BlueprintCallable, Category = "Network")
	bool CanPlayerInteract(const FString& PlayerID, EContainerPermission Permission) const;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetContainerOwner(const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetSharedAccess(bool bShared);

	// Delegates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantAdded, APlantActor*, Plant);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlantRemoved, int32, SlotIndex);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnContainerInteraction, const FString&, PlayerID, const FString&, Action);

	UPROPERTY(BlueprintAssignable)
	FOnPlantAdded OnPlantAdded;

	UPROPERTY(BlueprintAssignable)
	FOnPlantRemoved OnPlantRemoved;

	UPROPERTY(BlueprintAssignable)
	FOnEnvironmentalChange OnEnvironmentalChange;

	UPROPERTY(BlueprintAssignable)
	FOnContainerInteraction OnContainerInteraction;

private:
	// Network permission helpers
	class AHydroGrowNetworkGameMode* GetNetworkGameMode() const;
	bool HasPermission(const FString& PlayerID, EContainerPermission Permission) const;
};