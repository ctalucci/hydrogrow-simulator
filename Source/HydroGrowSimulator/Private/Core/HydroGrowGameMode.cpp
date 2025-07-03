#include "Core/HydroGrowGameMode.h"
#include "Core/HydroGrowPlayerController.h"
#include "Player/HydroGrowCharacter.h"
#include "Core/HydroGrowGameInstance.h"
#include "Systems/TimeManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"

AHydroGrowGameMode::AHydroGrowGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PlayerControllerClass = AHydroGrowPlayerController::StaticClass();
	DefaultPawnClass = AHydroGrowCharacter::StaticClass();
	
	// Default values
	PlayerLevel = 1;
	Experience = 0;
	Coins = 100;
	ResearchPoints = 0;
	EnergyCredits = 1000;
	
	BaseExperienceRequired = 100;
	ExperienceScalingFactor = 1.5f;
	StartingCoins = 100;
	DailyEnergyCredits = 1000;
}

void AHydroGrowGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	TimeManager = GetGameInstance()->GetSubsystem<UTimeManager>();
	
	if (!LoadGame())
	{
		InitializeNewGame();
	}
}

void AHydroGrowGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateEnergyCredits(DeltaTime);
}

void AHydroGrowGameMode::AddExperience(int32 ExperienceAmount)
{
	Experience += ExperienceAmount;
	
	while (CanLevelUp())
	{
		LevelUp();
	}
}

bool AHydroGrowGameMode::CanLevelUp() const
{
	return Experience >= GetExperienceForNextLevel();
}

void AHydroGrowGameMode::LevelUp()
{
	if (!CanLevelUp())
	{
		return;
	}
	
	Experience -= GetExperienceForNextLevel();
	PlayerLevel++;
	
	// Bonus coins for leveling up
	AddCoins(PlayerLevel * 10);
	
	// Bonus research points
	AddResearchPoints(PlayerLevel * 2);
	
	OnLevelUp.Broadcast(PlayerLevel);
	
	UE_LOG(LogTemp, Warning, TEXT("Level Up! New Level: %d"), PlayerLevel);
}

void AHydroGrowGameMode::AddCoins(int32 CoinAmount)
{
	Coins += CoinAmount;
	OnCurrencyChanged.Broadcast(TEXT("Coins"), Coins);
}

bool AHydroGrowGameMode::SpendCoins(int32 CoinAmount)
{
	if (Coins >= CoinAmount)
	{
		Coins -= CoinAmount;
		OnCurrencyChanged.Broadcast(TEXT("Coins"), Coins);
		return true;
	}
	return false;
}

void AHydroGrowGameMode::AddResearchPoints(int32 ResearchAmount)
{
	ResearchPoints += ResearchAmount;
	OnCurrencyChanged.Broadcast(TEXT("Research"), ResearchPoints);
}

bool AHydroGrowGameMode::SpendResearchPoints(int32 ResearchAmount)
{
	if (ResearchPoints >= ResearchAmount)
	{
		ResearchPoints -= ResearchAmount;
		OnCurrencyChanged.Broadcast(TEXT("Research"), ResearchPoints);
		return true;
	}
	return false;
}

void AHydroGrowGameMode::SaveGame()
{
	// Implementation will be completed when save system is implemented
	UE_LOG(LogTemp, Warning, TEXT("Saving game..."));
}

bool AHydroGrowGameMode::LoadGame()
{
	// Implementation will be completed when save system is implemented
	UE_LOG(LogTemp, Warning, TEXT("Loading game..."));
	return false; // Return false for now to initialize new game
}

int32 AHydroGrowGameMode::GetExperienceForNextLevel() const
{
	return FMath::RoundToInt(BaseExperienceRequired * FMath::Pow(ExperienceScalingFactor, PlayerLevel - 1));
}

void AHydroGrowGameMode::InitializeNewGame()
{
	PlayerLevel = 1;
	Experience = 0;
	Coins = StartingCoins;
	ResearchPoints = 0;
	EnergyCredits = DailyEnergyCredits;
	
	UE_LOG(LogTemp, Warning, TEXT("New game initialized"));
}

AActor* AHydroGrowGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Try to find a player start
	AActor* PlayerStart = Super::ChoosePlayerStart_Implementation(Player);
	
	if (PlayerStart)
	{
		return PlayerStart;
	}
	
	// If no player start found, create a default spawn location
	UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found, using default spawn location"));
	
	// Return nullptr to use default spawn location (0,0,0)
	return nullptr;
}

APawn* AHydroGrowGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn* NewPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
	
	if (AHydroGrowCharacter* Character = Cast<AHydroGrowCharacter>(NewPawn))
	{
		// Set up the character with default values
		Character->SetPlayerName(TEXT("Player"));
		UE_LOG(LogTemp, Warning, TEXT("Spawned HydroGrow Character: %s"), *Character->GetName());
	}
	
	return NewPawn;
}

void AHydroGrowGameMode::UpdateEnergyCredits(float DeltaTime)
{
	// Energy credits regenerate slowly over time
	// For now, just a simple regeneration system
	if (TimeManager)
	{
		// Regenerate energy based on game time
		float RegenRate = 10.0f; // Credits per game hour
		float GameDeltaTime = TimeManager->GetCurrentTimeScale() * DeltaTime;
		EnergyCredits += FMath::RoundToInt(RegenRate * GameDeltaTime / 3600.0f);
		
		// Cap at maximum
		EnergyCredits = FMath::Min(EnergyCredits, DailyEnergyCredits * 2);
	}
}