#include "Network/HydroGrowNetworkGameMode.h"
#include "Network/HydroGrowNetworkGameState.h"
#include "Network/HydroGrowNetworkPlayerState.h"
#include "Core/HydroGrowPlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameSession.h"

AHydroGrowNetworkGameMode::AHydroGrowNetworkGameMode()
{
	// Set network game state
	GameStateClass = AHydroGrowNetworkGameState::StaticClass();
	PlayerStateClass = AHydroGrowNetworkPlayerState::StaticClass();
	
	// Network settings
	MaxPlayers = 4;
	bRequireInvitation = false;
	bAllowVisitors = true;
	SessionTimeoutMinutes = 60.0f;
	
	// Enable replication
	bReplicates = true;
	
	UE_LOG(LogTemp, Warning, TEXT("HydroGrowNetworkGameMode initialized"));
}

void AHydroGrowNetworkGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Set up auto-save timer for multiplayer sessions
	GetWorldTimerManager().SetTimer(AutoSaveTimer, this, &AHydroGrowNetworkGameMode::AutoSaveSession, 300.0f, true); // Auto-save every 5 minutes
	
	UE_LOG(LogTemp, Warning, TEXT("Network game mode started, max players: %d"), MaxPlayers);
}

void AHydroGrowNetworkGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (NewPlayer)
	{
		InitializePlayerData(NewPlayer);
		
		// Broadcast system message about new player
		FString PlayerName = NewPlayer->GetPlayerState<APlayerState>()->GetPlayerName();
		BroadcastChatMessage(TEXT("System"), FString::Printf(TEXT("%s joined the garden"), *PlayerName), true);
		
		UE_LOG(LogTemp, Warning, TEXT("Player joined: %s"), *PlayerName);
	}
}

void AHydroGrowNetworkGameMode::Logout(AController* Exiting)
{
	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		FString PlayerName = PC->GetPlayerState<APlayerState>()->GetPlayerName();
		FString PlayerID = GeneratePlayerID(PC);
		
		// Broadcast system message about player leaving
		BroadcastChatMessage(TEXT("System"), FString::Printf(TEXT("%s left the garden"), *PlayerName), true);
		
		// Clean up player data
		CleanupPlayerData(PlayerID);
		
		// Fire event
		OnPlayerLeft.Broadcast(PlayerName);
		
		UE_LOG(LogTemp, Warning, TEXT("Player left: %s"), *PlayerName);
	}
	
	Super::Logout(Exiting);
}

bool AHydroGrowNetworkGameMode::SetPlayerRole(const FString& PlayerID, EPlayerRole NewRole)
{
	FNetworkPlayerData* PlayerData = ConnectedPlayers.Find(PlayerID);
	if (!PlayerData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found: %s"), *PlayerID);
		return false;
	}
	
	// Only owner can change roles
	if (SessionOwnerID != PlayerID && NewRole != EPlayerRole::Visitor)
	{
		// Check if requester is owner
		// This should be validated on the calling side
	}
	
	EPlayerRole OldRole = PlayerData->Role;
	PlayerData->Role = NewRole;
	
	// Update permissions based on role
	switch (NewRole)
	{
	case EPlayerRole::Owner:
		PlayerData->Permissions = FPlayerPermissions::GetOwnerPermissions();
		break;
	case EPlayerRole::Manager:
		PlayerData->Permissions = FPlayerPermissions::GetManagerPermissions();
		break;
	case EPlayerRole::Helper:
		PlayerData->Permissions = FPlayerPermissions::GetHelperPermissions();
		break;
	case EPlayerRole::Visitor:
		PlayerData->Permissions = FPlayerPermissions::GetVisitorPermissions();
		break;
	}
	
	// Log the role change
	LogNetworkAction(TEXT("System"), ENetworkAction::ManagePermissions, 
		FString::Printf(TEXT("%s role changed from %s to %s"), 
			*PlayerData->PlayerName, 
			*UEnum::GetValueAsString(OldRole),
			*UEnum::GetValueAsString(NewRole)));
	
	// Broadcast role change
	OnPlayerRoleChanged.Broadcast(*PlayerData);
	
	UE_LOG(LogTemp, Warning, TEXT("Player %s role changed to %s"), *PlayerData->PlayerName, *UEnum::GetValueAsString(NewRole));
	return true;
}

bool AHydroGrowNetworkGameMode::KickPlayer(const FString& PlayerID, const FString& Reason)
{
	FNetworkPlayerData* PlayerData = ConnectedPlayers.Find(PlayerID);
	if (!PlayerData)
	{
		return false;
	}
	
	// Find the player controller
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && GeneratePlayerID(PC) == PlayerID)
		{
			// Log the kick
			LogNetworkAction(TEXT("System"), ENetworkAction::ManagePermissions, 
				FString::Printf(TEXT("%s was kicked from the garden. Reason: %s"), *PlayerData->PlayerName, *Reason));
			
			// Broadcast system message
			BroadcastChatMessage(TEXT("System"), 
				FString::Printf(TEXT("%s was removed from the garden"), *PlayerData->PlayerName), true);
			
			// Kick the player
			GetGameSession()->KickPlayer(PC, FText::FromString(Reason));
			return true;
		}
	}
	
	return false;
}

void AHydroGrowNetworkGameMode::BroadcastChatMessage(const FString& PlayerName, const FString& Message, bool bIsSystemMessage)
{
	FNetworkChatMessage ChatMessage(PlayerName, Message, bIsSystemMessage);
	ChatHistory.Add(ChatMessage);
	
	// Keep chat history manageable
	if (ChatHistory.Num() > 100)
	{
		ChatHistory.RemoveAt(0);
	}
	
	OnChatMessage.Broadcast(ChatMessage);
	
	UE_LOG(LogTemp, Log, TEXT("Chat [%s]: %s"), *PlayerName, *Message);
}

void AHydroGrowNetworkGameMode::LogNetworkAction(const FString& PlayerName, ENetworkAction ActionType, const FString& Description, const FVector& Location)
{
	FNetworkActionLog ActionLog(PlayerName, ActionType, Description, Location);
	ActionHistory.Add(ActionLog);
	
	// Keep action history manageable
	if (ActionHistory.Num() > 500)
	{
		ActionHistory.RemoveAt(0);
	}
	
	// Update contribution score
	if (!PlayerName.Equals(TEXT("System")))
	{
		for (auto& PlayerPair : ConnectedPlayers)
		{
			if (PlayerPair.Value.PlayerName.Equals(PlayerName))
			{
				UpdatePlayerContributionScore(PlayerPair.Key, 1);
				break;
			}
		}
	}
	
	OnNetworkAction.Broadcast(ActionLog);
	
	UE_LOG(LogTemp, Log, TEXT("Action [%s]: %s - %s"), *PlayerName, *UEnum::GetValueAsString(ActionType), *Description);
}

bool AHydroGrowNetworkGameMode::CanPlayerPerformAction(const FString& PlayerID, ENetworkAction ActionType) const
{
	const FNetworkPlayerData* PlayerData = ConnectedPlayers.Find(PlayerID);
	if (!PlayerData)
	{
		return false;
	}
	
	return ValidatePlayerAction(PlayerID, ActionType);
}

void AHydroGrowNetworkGameMode::SaveMultiplayerSession()
{
	// This would integrate with the save system to save multiplayer session data
	UE_LOG(LogTemp, Warning, TEXT("Saving multiplayer session..."));
	
	// Save session would include:
	// - All player data and roles
	// - Shared garden state
	// - Chat and action history
	// - Session settings
}

bool AHydroGrowNetworkGameMode::LoadMultiplayerSession(const FString& SessionName)
{
	// This would load a previously saved multiplayer session
	UE_LOG(LogTemp, Warning, TEXT("Loading multiplayer session: %s"), *SessionName);
	
	// Load would restore:
	// - Player roles and permissions
	// - Garden state
	// - Any persistent chat/action history
	
	return true;
}

int32 AHydroGrowNetworkGameMode::GetCurrentPlayerCount() const
{
	return ConnectedPlayers.Num();
}

TArray<FNetworkPlayerData> AHydroGrowNetworkGameMode::GetAllPlayerData() const
{
	TArray<FNetworkPlayerData> PlayerDataArray;
	for (const auto& PlayerPair : ConnectedPlayers)
	{
		PlayerDataArray.Add(PlayerPair.Value);
	}
	return PlayerDataArray;
}

FNetworkPlayerData AHydroGrowNetworkGameMode::GetPlayerData(const FString& PlayerID) const
{
	const FNetworkPlayerData* PlayerData = ConnectedPlayers.Find(PlayerID);
	return PlayerData ? *PlayerData : FNetworkPlayerData();
}

void AHydroGrowNetworkGameMode::InitializePlayerData(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}
	
	FString PlayerID = GeneratePlayerID(PlayerController);
	FString PlayerName = PlayerController->GetPlayerState<APlayerState>()->GetPlayerName();
	
	FNetworkPlayerData NewPlayerData;
	NewPlayerData.PlayerName = PlayerName;
	NewPlayerData.PlayerID = PlayerID;
	NewPlayerData.bIsOnline = true;
	NewPlayerData.JoinTime = FDateTime::Now();
	NewPlayerData.ContributionScore = 0;
	
	// Assign role based on whether this is the first player (owner) or not
	if (ConnectedPlayers.Num() == 0)
	{
		NewPlayerData.Role = EPlayerRole::Owner;
		NewPlayerData.Permissions = FPlayerPermissions::GetOwnerPermissions();
		SessionOwnerID = PlayerID;
	}
	else if (bAllowVisitors)
	{
		NewPlayerData.Role = EPlayerRole::Helper;
		NewPlayerData.Permissions = FPlayerPermissions::GetHelperPermissions();
	}
	else
	{
		NewPlayerData.Role = EPlayerRole::Visitor;
		NewPlayerData.Permissions = FPlayerPermissions::GetVisitorPermissions();
	}
	
	ConnectedPlayers.Add(PlayerID, NewPlayerData);
	
	// Fire event
	OnPlayerJoined.Broadcast(NewPlayerData);
	
	UE_LOG(LogTemp, Warning, TEXT("Initialized player data for %s (Role: %s)"), 
		*PlayerName, *UEnum::GetValueAsString(NewPlayerData.Role));
}

void AHydroGrowNetworkGameMode::CleanupPlayerData(const FString& PlayerID)
{
	ConnectedPlayers.Remove(PlayerID);
	
	// If the owner leaves, promote another player
	if (SessionOwnerID == PlayerID && ConnectedPlayers.Num() > 0)
	{
		// Find the player with the highest contribution score to promote
		FString NewOwnerID;
		int32 HighestScore = -1;
		
		for (const auto& PlayerPair : ConnectedPlayers)
		{
			if (PlayerPair.Value.ContributionScore > HighestScore)
			{
				HighestScore = PlayerPair.Value.ContributionScore;
				NewOwnerID = PlayerPair.Key;
			}
		}
		
		if (!NewOwnerID.IsEmpty())
		{
			SessionOwnerID = NewOwnerID;
			SetPlayerRole(NewOwnerID, EPlayerRole::Owner);
			
			FNetworkPlayerData* NewOwnerData = ConnectedPlayers.Find(NewOwnerID);
			if (NewOwnerData)
			{
				BroadcastChatMessage(TEXT("System"), 
					FString::Printf(TEXT("%s is now the garden owner"), *NewOwnerData->PlayerName), true);
			}
		}
	}
}

void AHydroGrowNetworkGameMode::UpdatePlayerContributionScore(const FString& PlayerID, int32 ScoreChange)
{
	FNetworkPlayerData* PlayerData = ConnectedPlayers.Find(PlayerID);
	if (PlayerData)
	{
		PlayerData->ContributionScore += ScoreChange;
	}
}

FString AHydroGrowNetworkGameMode::GeneratePlayerID(APlayerController* PlayerController) const
{
	if (PlayerController && PlayerController->GetPlayerState<APlayerState>())
	{
		return FString::Printf(TEXT("%d_%s"), 
			PlayerController->GetPlayerState<APlayerState>()->GetPlayerId(),
			*PlayerController->GetPlayerState<APlayerState>()->GetPlayerName());
	}
	return TEXT("Unknown");
}

bool AHydroGrowNetworkGameMode::ValidatePlayerAction(const FString& PlayerID, ENetworkAction ActionType) const
{
	const FNetworkPlayerData* PlayerData = ConnectedPlayers.Find(PlayerID);
	if (!PlayerData)
	{
		return false;
	}
	
	const FPlayerPermissions& Permissions = PlayerData->Permissions;
	
	switch (ActionType)
	{
	case ENetworkAction::PlantSeed:
		return Permissions.bCanPlantSeeds;
	case ENetworkAction::HarvestPlant:
		return Permissions.bCanHarvestPlants;
	case ENetworkAction::AdjustPH:
	case ENetworkAction::AddNutrients:
	case ENetworkAction::TogglePump:
		return Permissions.bCanAdjustEnvironment;
	case ENetworkAction::PurchaseEquipment:
		return Permissions.bCanPurchaseEquipment;
	case ENetworkAction::SellProduce:
		return Permissions.bCanSellProduce;
	case ENetworkAction::ManagePermissions:
		return Permissions.bCanManagePermissions;
	default:
		return false;
	}
}

void AHydroGrowNetworkGameMode::AutoSaveSession()
{
	if (GetCurrentPlayerCount() > 0)
	{
		SaveMultiplayerSession();
	}
}