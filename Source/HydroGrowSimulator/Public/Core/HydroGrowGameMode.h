#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HydroGrowTypes.h"
#include "HydroGrowGameMode.generated.h"

class UHydroGrowGameInstance;
class AHydroGrowPlayerController;
class UTimeManager;

UCLASS()
class HYDROGROWSIMULATOR_API AHydroGrowGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AHydroGrowGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Spawn system overrides
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddExperience(int32 ExperienceAmount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool CanLevelUp() const;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void LevelUp();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AddCoins(int32 CoinAmount);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool SpendCoins(int32 CoinAmount);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AddResearchPoints(int32 ResearchAmount);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	bool SpendResearchPoints(int32 ResearchAmount);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	bool LoadGame();

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetPlayerLevel() const { return PlayerLevel; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetExperience() const { return Experience; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetExperienceForNextLevel() const;

	UFUNCTION(BlueprintPure, Category = "Economy")
	int32 GetCoins() const { return Coins; }

	UFUNCTION(BlueprintPure, Category = "Economy")
	int32 GetResearchPoints() const { return ResearchPoints; }

	UFUNCTION(BlueprintPure, Category = "Economy")
	int32 GetEnergyCredits() const { return EnergyCredits; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	int32 PlayerLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progression")
	int32 Experience;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
	int32 ResearchPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
	int32 EnergyCredits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
	int32 BaseExperienceRequired;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
	float ExperienceScalingFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Economy")
	int32 StartingCoins;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Economy")
	int32 DailyEnergyCredits;

private:
	UPROPERTY()
	UTimeManager* TimeManager;

	void InitializeNewGame();
	void UpdateEnergyCredits(float DeltaTime);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, FString, CurrencyType, int32, NewAmount);

	UPROPERTY(BlueprintAssignable)
	FOnLevelUp OnLevelUp;

	UPROPERTY(BlueprintAssignable)
	FOnCurrencyChanged OnCurrencyChanged;
};