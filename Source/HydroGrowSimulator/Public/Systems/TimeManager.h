#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimeManager.generated.h"

UENUM(BlueprintType)
enum class EGameTimeMode : uint8
{
	Paused		UMETA(DisplayName = "Paused"),
	Normal		UMETA(DisplayName = "Normal Speed"),
	Fast		UMETA(DisplayName = "2x Speed"),
	VeryFast	UMETA(DisplayName = "4x Speed"),
	Accelerated	UMETA(DisplayName = "Accelerated Mode")
};

USTRUCT(BlueprintType)
struct FGameDateTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time")
	int32 Year;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time")
	int32 Month;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time")
	int32 Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time")
	int32 Hour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time")
	int32 Minute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time")
	int32 Second;

	FGameDateTime()
	{
		Year = 2025;
		Month = 1;
		Day = 1;
		Hour = 8;
		Minute = 0;
		Second = 0;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d/%d/%d %02d:%02d:%02d"), Month, Day, Year, Hour, Minute, Second);
	}
};

UCLASS()
class HYDROGROWSIMULATOR_API UTimeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Time Management")
	void SetTimeMode(EGameTimeMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "Time Management")
	void ToggleTimeMode();

	UFUNCTION(BlueprintCallable, Category = "Time Management")
	void PauseTime();

	UFUNCTION(BlueprintCallable, Category = "Time Management")
	void ResumeTime();

	UFUNCTION(BlueprintPure, Category = "Time Management")
	EGameTimeMode GetCurrentTimeMode() const { return CurrentTimeMode; }

	UFUNCTION(BlueprintPure, Category = "Time Management")
	float GetCurrentTimeScale() const;

	UFUNCTION(BlueprintPure, Category = "Time Management")
	bool IsTimePaused() const { return CurrentTimeMode == EGameTimeMode::Paused; }

	UFUNCTION(BlueprintPure, Category = "Game Time")
	FGameDateTime GetGameDateTime() const { return CurrentGameTime; }

	UFUNCTION(BlueprintPure, Category = "Game Time")
	float GetTimeOfDay() const;

	UFUNCTION(BlueprintPure, Category = "Game Time")
	int32 GetCurrentDay() const;

	UFUNCTION(BlueprintPure, Category = "Game Time")
	FString GetFormattedGameTime() const;

	UFUNCTION(BlueprintPure, Category = "Game Time")
	bool IsDay() const;

	UFUNCTION(BlueprintPure, Category = "Game Time")
	bool IsNight() const;

	UFUNCTION(BlueprintCallable, Category = "Offline Progress")
	void ProcessOfflineTime(const FDateTime& LastSaveTime);

	UFUNCTION(BlueprintPure, Category = "Offline Progress")
	float GetOfflineHours() const { return OfflineHoursProcessed; }

	UFUNCTION(BlueprintCallable, Category = "Game Time")
	void AdvanceTime(float Hours);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time State")
	EGameTimeMode CurrentTimeMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time State")
	EGameTimeMode PreviousTimeMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Time")
	FGameDateTime CurrentGameTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Time")
	float TotalGameTimeElapsed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Offline Progress")
	float OfflineHoursProcessed;

	// Configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TMap<EGameTimeMode, float> TimeModeScales;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float DayStartHour;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float NightStartHour;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float MaxOfflineHours;

private:
	FTimerHandle GameTimeTimer;
	float AccumulatedTime;

	void UpdateGameTime();
	void InitializeTimeScales();
	void BroadcastTimeEvents();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeModeChanged, EGameTimeMode, NewMode);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewDay, int32, DayNumber);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, Hour);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayNightCycle, bool, bIsDay);

	UPROPERTY(BlueprintAssignable)
	FOnTimeModeChanged OnTimeModeChanged;

	UPROPERTY(BlueprintAssignable)
	FOnNewDay OnNewDay;

	UPROPERTY(BlueprintAssignable)
	FOnHourChanged OnHourChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDayNightCycle OnDayNightCycle;
};