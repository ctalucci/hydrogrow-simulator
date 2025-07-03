#include "Network/HydroGrowNetworkGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AHydroGrowNetworkGameState::AHydroGrowNetworkGameState()
{
	// Enable replication
	bReplicates = true;
	bAlwaysRelevant = true;
	
	// Initialize shared resources
	SharedCoins = 100;
	SharedResearchPoints = 0;
	SharedEnergyCredits = 1000;
	
	// Initialize session info
	SessionStartTime = FDateTime::Now();
	bIsPrivateSession = false;
	MaxPlayers = 4;
	SessionName = TEXT("HydroGrow Garden");
	
	// Initialize shared time
	SharedGameTime = FGameDateTime();
	SharedTimeMode = EGameTimeMode::Normal;
}

void AHydroGrowNetworkGameState::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("HydroGrow Network GameState initialized"));
}

void AHydroGrowNetworkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate to all clients
	DOREPLIFETIME(AHydroGrowNetworkGameState, ConnectedPlayers);
	DOREPLIFETIME(AHydroGrowNetworkGameState, ChatHistory);
	DOREPLIFETIME(AHydroGrowNetworkGameState, ActionHistory);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SharedGameTime);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SharedTimeMode);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SharedCoins);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SharedResearchPoints);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SharedEnergyCredits);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SessionName);
	DOREPLIFETIME(AHydroGrowNetworkGameState, SessionStartTime);
	DOREPLIFETIME(AHydroGrowNetworkGameState, bIsPrivateSession);
	DOREPLIFETIME(AHydroGrowNetworkGameState, MaxPlayers);
}

void AHydroGrowNetworkGameState::UpdateSharedResources(int32 Coins, int32 Research, int32 Energy)
{
	if (HasAuthority())
	{
		SharedCoins = Coins;
		SharedResearchPoints = Research;
		SharedEnergyCredits = Energy;
		
		// Trigger replication
		OnRep_SharedResources();
	}
}

void AHydroGrowNetworkGameState::UpdateSharedTime(const FGameDateTime& NewTime, EGameTimeMode NewMode)
{
	if (HasAuthority())
	{
		SharedGameTime = NewTime;
		SharedTimeMode = NewMode;
		
		// Trigger replication
		OnRep_SharedTime();
	}
}

void AHydroGrowNetworkGameState::AddChatMessage(const FNetworkChatMessage& Message)
{
	if (HasAuthority())
	{
		ChatHistory.Add(Message);
		
		// Keep chat history manageable
		if (ChatHistory.Num() > 100)
		{
			ChatHistory.RemoveAt(0);
		}
		
		// Trigger replication
		OnRep_ChatHistory();
	}
}

void AHydroGrowNetworkGameState::AddActionLog(const FNetworkActionLog& ActionLog)
{
	if (HasAuthority())
	{
		ActionHistory.Add(ActionLog);
		
		// Keep action history manageable
		if (ActionHistory.Num() > 500)
		{
			ActionHistory.RemoveAt(0);
		}
		
		// Trigger replication
		OnRep_ActionHistory();
	}
}

void AHydroGrowNetworkGameState::UpdatePlayerList(const TArray<FNetworkPlayerData>& Players)
{
	if (HasAuthority())
	{
		ConnectedPlayers = Players;
		
		// Trigger replication
		OnRep_ConnectedPlayers();
	}
}

void AHydroGrowNetworkGameState::OnRep_ConnectedPlayers()
{
	// Broadcast player list changes
	UE_LOG(LogTemp, Log, TEXT("Player list updated: %d players"), ConnectedPlayers.Num());
	
	// You could fire specific events here for player joins/leaves
	// by comparing with previous state
}

void AHydroGrowNetworkGameState::OnRep_ChatHistory()
{
	// Broadcast new chat message
	if (ChatHistory.Num() > 0)
	{
		const FNetworkChatMessage& LatestMessage = ChatHistory.Last();
		OnChatMessageNetwork.Broadcast(LatestMessage);
		
		UE_LOG(LogTemp, Log, TEXT("New chat message: [%s] %s"), 
			*LatestMessage.PlayerName, *LatestMessage.Message);
	}
}

void AHydroGrowNetworkGameState::OnRep_ActionHistory()
{
	// Broadcast new action
	if (ActionHistory.Num() > 0)
	{
		const FNetworkActionLog& LatestAction = ActionHistory.Last();
		OnNetworkActionNetwork.Broadcast(LatestAction);
		
		UE_LOG(LogTemp, Log, TEXT("New network action: [%s] %s"), 
			*LatestAction.PlayerName, *LatestAction.ActionDescription);
	}
}

void AHydroGrowNetworkGameState::OnRep_SharedTime()
{
	// Broadcast time change
	OnSharedTimeChanged.Broadcast(SharedGameTime, SharedTimeMode);
	
	UE_LOG(LogTemp, Log, TEXT("Shared time updated: %s (Mode: %s)"), 
		*SharedGameTime.ToString(), *UEnum::GetValueAsString(SharedTimeMode));
}

void AHydroGrowNetworkGameState::OnRep_SharedResources()
{
	// Broadcast resource change
	OnSharedResourcesChanged.Broadcast(SharedCoins, SharedResearchPoints);
	
	UE_LOG(LogTemp, Log, TEXT("Shared resources updated: %d coins, %d research, %d energy"), 
		SharedCoins, SharedResearchPoints, SharedEnergyCredits);
}