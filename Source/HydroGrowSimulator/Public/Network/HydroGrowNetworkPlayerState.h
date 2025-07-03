#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Network/HydroGrowNetworkTypes.h"
#include "HydroGrowNetworkPlayerState.generated.h"

UCLASS()
class HYDROGROWSIMULATOR_API AHydroGrowNetworkPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AHydroGrowNetworkPlayerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintPure, Category = "Network Player")
	EPlayerRole GetPlayerRole() const { return PlayerRole; }

	UFUNCTION(BlueprintPure, Category = "Network Player")
	const FPlayerPermissions& GetPlayerPermissions() const { return PlayerPermissions; }

	UFUNCTION(BlueprintPure, Category = "Network Player")
	int32 GetContributionScore() const { return ContributionScore; }

	UFUNCTION(BlueprintPure, Category = "Network Player")
	FDateTime GetJoinTime() const { return JoinTime; }

	UFUNCTION(BlueprintPure, Category = "Network Player")
	bool HasPermission(ENetworkAction ActionType) const;

	UFUNCTION(BlueprintCallable, Category = "Network Player")
	void SetPlayerRole(EPlayerRole NewRole);

	UFUNCTION(BlueprintCallable, Category = "Network Player")
	void AddContributionScore(int32 Points);

protected:
	// Replicated player data
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network Player")
	EPlayerRole PlayerRole;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network Player")
	FPlayerPermissions PlayerPermissions;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network Player")
	int32 ContributionScore;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network Player")
	FDateTime JoinTime;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Network Player")
	bool bIsGardenOwner;

private:
	// Replication callbacks
	UFUNCTION()
	void OnRep_PlayerRole();

	UFUNCTION()
	void OnRep_ContributionScore();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerRoleChanged, EPlayerRole, OldRole, EPlayerRole, NewRole);
	UPROPERTY(BlueprintAssignable)
	FOnPlayerRoleChanged OnPlayerRoleChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContributionScoreChanged, int32, NewScore);
	UPROPERTY(BlueprintAssignable)
	FOnContributionScoreChanged OnContributionScoreChanged;
};