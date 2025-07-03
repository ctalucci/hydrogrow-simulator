#include "Systems/TimeManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

void UTimeManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Initialize default values
	CurrentTimeMode = EGameTimeMode::Normal;
	PreviousTimeMode = EGameTimeMode::Normal;
	TotalGameTimeElapsed = 0.0f;
	AccumulatedTime = 0.0f;
	OfflineHoursProcessed = 0.0f;
	
	// Configuration
	DayStartHour = 6.0f;
	NightStartHour = 18.0f;
	MaxOfflineHours = 72.0f; // Maximum 3 days of offline progression
	
	InitializeTimeScales();
	
	// Start the game time update timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(GameTimeTimer, this, &UTimeManager::UpdateGameTime, 0.1f, true);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("TimeManager initialized"));
}

void UTimeManager::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GameTimeTimer);
	}
	
	Super::Deinitialize();
}

void UTimeManager::SetTimeMode(EGameTimeMode NewMode)
{
	if (CurrentTimeMode != NewMode)
	{
		PreviousTimeMode = CurrentTimeMode;
		CurrentTimeMode = NewMode;
		
		OnTimeModeChanged.Broadcast(CurrentTimeMode);
		
		UE_LOG(LogTemp, Warning, TEXT("Time mode changed to: %s"), *UEnum::GetValueAsString(CurrentTimeMode));
	}
}

void UTimeManager::ToggleTimeMode()
{
	switch (CurrentTimeMode)
	{
	case EGameTimeMode::Paused:
		SetTimeMode(EGameTimeMode::Normal);
		break;
	case EGameTimeMode::Normal:
		SetTimeMode(EGameTimeMode::Fast);
		break;
	case EGameTimeMode::Fast:
		SetTimeMode(EGameTimeMode::VeryFast);
		break;
	case EGameTimeMode::VeryFast:
		SetTimeMode(EGameTimeMode::Normal);
		break;
	case EGameTimeMode::Accelerated:
		SetTimeMode(EGameTimeMode::Normal);
		break;
	}
}

void UTimeManager::PauseTime()
{
	if (CurrentTimeMode != EGameTimeMode::Paused)
	{
		PreviousTimeMode = CurrentTimeMode;
		SetTimeMode(EGameTimeMode::Paused);
	}
}

void UTimeManager::ResumeTime()
{
	if (CurrentTimeMode == EGameTimeMode::Paused)
	{
		SetTimeMode(PreviousTimeMode);
	}
}

float UTimeManager::GetCurrentTimeScale() const
{
	const float* TimeScale = TimeModeScales.Find(CurrentTimeMode);
	return TimeScale ? *TimeScale : 1.0f;
}

float UTimeManager::GetTimeOfDay() const
{
	return CurrentGameTime.Hour + (CurrentGameTime.Minute / 60.0f) + (CurrentGameTime.Second / 3600.0f);
}

int32 UTimeManager::GetCurrentDay() const
{
	// Calculate total days since start of game
	int32 TotalDays = (CurrentGameTime.Year - 2025) * 365 + (CurrentGameTime.Month - 1) * 30 + (CurrentGameTime.Day - 1);
	return TotalDays + 1; // Start from day 1
}

FString UTimeManager::GetFormattedGameTime() const
{
	return CurrentGameTime.ToString();
}

bool UTimeManager::IsDay() const
{
	float TimeOfDay = GetTimeOfDay();
	return TimeOfDay >= DayStartHour && TimeOfDay < NightStartHour;
}

bool UTimeManager::IsNight() const
{
	return !IsDay();
}

void UTimeManager::ProcessOfflineTime(const FDateTime& LastSaveTime)
{
	FDateTime CurrentTime = FDateTime::Now();
	FTimespan OfflineTime = CurrentTime - LastSaveTime;
	
	float OfflineHours = OfflineTime.GetTotalHours();
	
	// Cap offline progression
	OfflineHours = FMath::Min(OfflineHours, MaxOfflineHours);
	
	if (OfflineHours > 0.1f) // Only process if more than 6 minutes offline
	{
		AdvanceTime(OfflineHours);
		OfflineHoursProcessed = OfflineHours;
		
		UE_LOG(LogTemp, Warning, TEXT("Processed %.2f hours of offline time"), OfflineHours);
	}
	else
	{
		OfflineHoursProcessed = 0.0f;
	}
}

void UTimeManager::AdvanceTime(float Hours)
{
	int32 PreviousDay = GetCurrentDay();
	int32 PreviousHour = CurrentGameTime.Hour;
	bool WasDay = IsDay();
	
	TotalGameTimeElapsed += Hours;
	
	// Convert hours to seconds and add to current time
	float TotalSeconds = Hours * 3600.0f;
	
	CurrentGameTime.Second += FMath::FloorToInt(TotalSeconds);
	
	// Handle overflow
	if (CurrentGameTime.Second >= 60)
	{
		CurrentGameTime.Minute += CurrentGameTime.Second / 60;
		CurrentGameTime.Second %= 60;
	}
	
	if (CurrentGameTime.Minute >= 60)
	{
		CurrentGameTime.Hour += CurrentGameTime.Minute / 60;
		CurrentGameTime.Minute %= 60;
	}
	
	if (CurrentGameTime.Hour >= 24)
	{
		CurrentGameTime.Day += CurrentGameTime.Hour / 24;
		CurrentGameTime.Hour %= 24;
	}
	
	// Handle month/year overflow (simplified)
	if (CurrentGameTime.Day > 30)
	{
		CurrentGameTime.Month += (CurrentGameTime.Day - 1) / 30;
		CurrentGameTime.Day = ((CurrentGameTime.Day - 1) % 30) + 1;
	}
	
	if (CurrentGameTime.Month > 12)
	{
		CurrentGameTime.Year += (CurrentGameTime.Month - 1) / 12;
		CurrentGameTime.Month = ((CurrentGameTime.Month - 1) % 12) + 1;
	}
	
	// Broadcast events
	int32 NewDay = GetCurrentDay();
	if (NewDay != PreviousDay)
	{
		OnNewDay.Broadcast(NewDay);
	}
	
	if (CurrentGameTime.Hour != PreviousHour)
	{
		OnHourChanged.Broadcast(CurrentGameTime.Hour);
	}
	
	bool IsCurrentlyDay = IsDay();
	if (IsCurrentlyDay != WasDay)
	{
		OnDayNightCycle.Broadcast(IsCurrentlyDay);
	}
}

void UTimeManager::UpdateGameTime()
{
	if (CurrentTimeMode == EGameTimeMode::Paused)
	{
		return;
	}
	
	float TimeScale = GetCurrentTimeScale();
	float DeltaTime = 0.1f; // Timer interval
	
	AccumulatedTime += DeltaTime * TimeScale;
	
	// Convert to game hours and advance time
	float GameHours = AccumulatedTime / 3600.0f;
	if (GameHours >= 0.01f) // Update every 0.01 game hours (36 seconds)
	{
		AdvanceTime(GameHours);
		AccumulatedTime = 0.0f;
	}
}

void UTimeManager::InitializeTimeScales()
{
	TimeModeScales.Empty();
	TimeModeScales.Add(EGameTimeMode::Paused, 0.0f);
	TimeModeScales.Add(EGameTimeMode::Normal, 60.0f);        // 1 real minute = 1 game hour
	TimeModeScales.Add(EGameTimeMode::Fast, 120.0f);         // 2x speed
	TimeModeScales.Add(EGameTimeMode::VeryFast, 240.0f);     // 4x speed
	TimeModeScales.Add(EGameTimeMode::Accelerated, 1440.0f); // 1 real minute = 1 game day
}

void UTimeManager::BroadcastTimeEvents()
{
	// Additional time-based events can be triggered here
	// For example: seasonal changes, market fluctuations, etc.
}