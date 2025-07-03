#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Network/HydroGrowNetworkTypes.h"
#include "Core/HydroGrowTypes.h"
#include "Systems/TimeManager.h"
#include "HydroGrowNetworkGameState.generated.h"

UCLASS()
class HYDROGROWSIMULATOR_API AHydroGrowNetworkGameState : public AGameState
{
	GENERATED_BODY()

public:
	AHydroGrowNetworkGameState();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintPure, Category = "Network State")
	const TArray<FNetworkPlayerData>& GetConnectedPlayers() const { return ConnectedPlayers; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	const TArray<FNetworkChatMessage>& GetChatHistory() const { return ChatHistory; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	const TArray<FNetworkActionLog>& GetActionHistory() const { return ActionHistory; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	FGameDateTime GetSharedGameTime() const { return SharedGameTime; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	EGameTimeMode GetSharedTimeMode() const { return SharedTimeMode; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	int32 GetSharedCoins() const { return SharedCoins; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	int32 GetSharedResearchPoints() const { return SharedResearchPoints; }

	UFUNCTION(BlueprintPure, Category = "Network State")
	int32 GetSharedEnergyCredits() const { return SharedEnergyCredits; }

	UFUNCTION(BlueprintCallable, Category = "Network State")
	void UpdateSharedResources(int32 Coins, int32 Research, int32 Energy);

	UFUNCTION(BlueprintCallable, Category = "Network State")
	void UpdateSharedTime(const FGameDateTime& NewTime, EGameTimeMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "Network State")
	void AddChatMessage(const FNetworkChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Network State")
	void AddActionLog(const FNetworkActionLog& ActionLog);

	UFUNCTION(BlueprintCallable, Category = "Network State")
	void UpdatePlayerList(const TArray<FNetworkPlayerData>& Players);

protected:
	// Replicated shared state
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared State")
	TArray<FNetworkPlayerData> ConnectedPlayers;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared State")
	TArray<FNetworkChatMessage> ChatHistory;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared State")
	TArray<FNetworkActionLog> ActionHistory;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared Time")
	FGameDateTime SharedGameTime;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared Time")
	EGameTimeMode SharedTimeMode;

	// Shared resources (everyone contributes to and benefits from)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared Resources")
	int32 SharedCoins;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared Resources")
	int32 SharedResearchPoints;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Shared Resources")
	int32 SharedEnergyCredits;

	// Session information
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Session Info")
	FString SessionName;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Session Info")
	FDateTime SessionStartTime;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Session Info")
	bool bIsPrivateSession;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Session Info")
	int32 MaxPlayers;

private:
	// Replication events
	UFUNCTION()
	void OnRep_ConnectedPlayers();

	UFUNCTION()
	void OnRep_ChatHistory();

	UFUNCTION()
	void OnRep_ActionHistory();

	UFUNCTION()
	void OnRep_SharedTime();

	UFUNCTION()
	void OnRep_SharedResources();

public:
	// Network events (replicated to all clients)
	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoinedNetwork;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLeft OnPlayerLeftNetwork;

	UPROPERTY(BlueprintAssignable)
	FOnChatMessage OnChatMessageNetwork;

	UPROPERTY(BlueprintAssignable)
	FOnNetworkAction OnNetworkActionNetwork;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSharedResourcesChanged, int32, Coins, int32, Research);
	UPROPERTY(BlueprintAssignable)
	FOnSharedResourcesChanged OnSharedResourcesChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSharedTimeChanged, FGameDateTime, NewTime, EGameTimeMode, NewMode);
	UPROPERTY(BlueprintAssignable)
	FOnSharedTimeChanged OnSharedTimeChanged;
};