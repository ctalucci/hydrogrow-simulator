#include "Network/HydroGrowNetworkPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AHydroGrowNetworkPlayerState::AHydroGrowNetworkPlayerState()
{
	// Enable replication
	bReplicates = true;
	
	// Initialize default values
	PlayerRole = EPlayerRole::Helper;
	PlayerPermissions = FPlayerPermissions::GetHelperPermissions();
	ContributionScore = 0;
	JoinTime = FDateTime::Now();
	bIsGardenOwner = false;
}

void AHydroGrowNetworkPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate to all clients
	DOREPLIFETIME(AHydroGrowNetworkPlayerState, PlayerRole);
	DOREPLIFETIME(AHydroGrowNetworkPlayerState, PlayerPermissions);
	DOREPLIFETIME(AHydroGrowNetworkPlayerState, ContributionScore);
	DOREPLIFETIME(AHydroGrowNetworkPlayerState, JoinTime);
	DOREPLIFETIME(AHydroGrowNetworkPlayerState, bIsGardenOwner);
}

bool AHydroGrowNetworkPlayerState::HasPermission(ENetworkAction ActionType) const
{
	switch (ActionType)
	{
	case ENetworkAction::PlantSeed:
		return PlayerPermissions.bCanPlantSeeds;
	case ENetworkAction::HarvestPlant:
		return PlayerPermissions.bCanHarvestPlants;
	case ENetworkAction::AdjustPH:
	case ENetworkAction::AddNutrients:
	case ENetworkAction::TogglePump:
		return PlayerPermissions.bCanAdjustEnvironment;
	case ENetworkAction::PurchaseEquipment:
		return PlayerPermissions.bCanPurchaseEquipment;
	case ENetworkAction::SellProduce:
		return PlayerPermissions.bCanSellProduce;
	case ENetworkAction::ManagePermissions:
		return PlayerPermissions.bCanManagePermissions;
	default:
		return false;
	}
}

void AHydroGrowNetworkPlayerState::SetPlayerRole(EPlayerRole NewRole)
{
	if (HasAuthority())
	{
		EPlayerRole OldRole = PlayerRole;
		PlayerRole = NewRole;
		
		// Update permissions based on role
		switch (NewRole)
		{
		case EPlayerRole::Owner:
			PlayerPermissions = FPlayerPermissions::GetOwnerPermissions();
			bIsGardenOwner = true;
			break;
		case EPlayerRole::Manager:
			PlayerPermissions = FPlayerPermissions::GetManagerPermissions();
			bIsGardenOwner = false;
			break;
		case EPlayerRole::Helper:
			PlayerPermissions = FPlayerPermissions::GetHelperPermissions();
			bIsGardenOwner = false;
			break;
		case EPlayerRole::Visitor:
			PlayerPermissions = FPlayerPermissions::GetVisitorPermissions();
			bIsGardenOwner = false;
			break;
		}
		
		// Trigger replication
		OnRep_PlayerRole();
		
		UE_LOG(LogTemp, Warning, TEXT("Player %s role changed from %s to %s"), 
			*GetPlayerName(), *UEnum::GetValueAsString(OldRole), *UEnum::GetValueAsString(NewRole));
	}
}

void AHydroGrowNetworkPlayerState::AddContributionScore(int32 Points)
{
	if (HasAuthority())
	{
		ContributionScore += Points;
		
		// Trigger replication
		OnRep_ContributionScore();
		
		UE_LOG(LogTemp, Log, TEXT("Player %s contribution score: %d (+%d)"), 
			*GetPlayerName(), ContributionScore, Points);
	}
}

void AHydroGrowNetworkPlayerState::OnRep_PlayerRole()
{
	// Broadcast role change event
	// Note: We can't easily get the old role here, so we pass the current role twice
	OnPlayerRoleChanged.Broadcast(PlayerRole, PlayerRole);
	
	UE_LOG(LogTemp, Log, TEXT("Player %s role replicated: %s"), 
		*GetPlayerName(), *UEnum::GetValueAsString(PlayerRole));
}

void AHydroGrowNetworkPlayerState::OnRep_ContributionScore()
{
	// Broadcast score change
	OnContributionScoreChanged.Broadcast(ContributionScore);
	
	UE_LOG(LogTemp, Log, TEXT("Player %s contribution score replicated: %d"), 
		*GetPlayerName(), ContributionScore);
}