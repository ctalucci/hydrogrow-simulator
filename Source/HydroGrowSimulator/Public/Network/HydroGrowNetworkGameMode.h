#pragma once

#include "CoreMinimal.h"
#include "Core/HydroGrowGameMode.h"
#include "Network/HydroGrowNetworkTypes.h"
#include "Engine/GameSession.h"
#include "HydroGrowNetworkGameMode.generated.h"

class AHydroGrowNetworkGameState;
class AHydroGrowNetworkPlayerState;

UCLASS()
class HYDROGROWSIMULATOR_API AHydroGrowNetworkGameMode : public AHydroGrowGameMode
{
	GENERATED_BODY()

public:
	AHydroGrowNetworkGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Network Management")
	bool SetPlayerRole(const FString& PlayerID, EPlayerRole NewRole);

	UFUNCTION(BlueprintCallable, Category = "Network Management")
	bool KickPlayer(const FString& PlayerID, const FString& Reason);

	UFUNCTION(BlueprintCallable, Category = "Network Management")
	void BroadcastChatMessage(const FString& PlayerName, const FString& Message, bool bIsSystemMessage = false);

	UFUNCTION(BlueprintCallable, Category = "Network Management")
	void LogNetworkAction(const FString& PlayerName, ENetworkAction ActionType, const FString& Description, const FVector& Location = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Network Management")
	bool CanPlayerPerformAction(const FString& PlayerID, ENetworkAction ActionType) const;

	UFUNCTION(BlueprintCallable, Category = "Session Management")
	void SaveMultiplayerSession();

	UFUNCTION(BlueprintCallable, Category = "Session Management")
	bool LoadMultiplayerSession(const FString& SessionName);

	UFUNCTION(BlueprintPure, Category = "Network Management")
	int32 GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION(BlueprintPure, Category = "Network Management")
	int32 GetCurrentPlayerCount() const;

	UFUNCTION(BlueprintPure, Category = "Network Management")
	TArray<FNetworkPlayerData> GetAllPlayerData() const;

	UFUNCTION(BlueprintPure, Category = "Network Management")
	FNetworkPlayerData GetPlayerData(const FString& PlayerID) const;

protected:
	// Network configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network Settings")
	int32 MaxPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network Settings")
	bool bRequireInvitation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network Settings")
	bool bAllowVisitors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network Settings")
	float SessionTimeoutMinutes;

	// Player management
	UPROPERTY()
	TMap<FString, FNetworkPlayerData> ConnectedPlayers;

	UPROPERTY()
	TArray<FNetworkActionLog> ActionHistory;

	UPROPERTY()
	TArray<FNetworkChatMessage> ChatHistory;

	UPROPERTY()
	FString SessionOwnerID;

private:
	void InitializePlayerData(APlayerController* PlayerController);
	void CleanupPlayerData(const FString& PlayerID);
	void UpdatePlayerContributionScore(const FString& PlayerID, int32 ScoreChange);
	FString GeneratePlayerID(APlayerController* PlayerController) const;
	bool ValidatePlayerAction(const FString& PlayerID, ENetworkAction ActionType) const;

	// Session management
	void AutoSaveSession();
	FTimerHandle AutoSaveTimer;

public:
	// Network events
	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLeft OnPlayerLeft;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerRoleChanged OnPlayerRoleChanged;

	UPROPERTY(BlueprintAssignable)
	FOnChatMessage OnChatMessage;

	UPROPERTY(BlueprintAssignable)
	FOnNetworkAction OnNetworkAction;
};