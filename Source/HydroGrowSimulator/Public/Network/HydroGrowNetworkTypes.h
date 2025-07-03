#pragma once

#include "CoreMinimal.h"
#include "Engine/NetSerialization.h"
#include "Core/HydroGrowTypes.h"
#include "HydroGrowNetworkTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	None		UMETA(DisplayName = "None"),
	Owner		UMETA(DisplayName = "Garden Owner"),
	Manager		UMETA(DisplayName = "Manager"),
	Helper		UMETA(DisplayName = "Helper"),
	Visitor		UMETA(DisplayName = "Visitor")
};

UENUM(BlueprintType)
enum class ENetworkAction : uint8
{
	PlantSeed			UMETA(DisplayName = "Plant Seed"),
	HarvestPlant		UMETA(DisplayName = "Harvest Plant"),
	AdjustPH			UMETA(DisplayName = "Adjust pH"),
	AddNutrients		UMETA(DisplayName = "Add Nutrients"),
	TogglePump			UMETA(DisplayName = "Toggle Pump"),
	PurchaseEquipment	UMETA(DisplayName = "Purchase Equipment"),
	SellProduce			UMETA(DisplayName = "Sell Produce"),
	ManagePermissions	UMETA(DisplayName = "Manage Permissions")
};

UENUM(BlueprintType)
enum class EContainerPermission : uint8
{
	PlantSeeds			UMETA(DisplayName = "Plant Seeds"),
	HarvestPlants		UMETA(DisplayName = "Harvest Plants"),
	AdjustEnvironment	UMETA(DisplayName = "Adjust Environment"),
	AddNutrients		UMETA(DisplayName = "Add Nutrients"),
	OperateEquipment	UMETA(DisplayName = "Operate Equipment")
};

USTRUCT(BlueprintType)
struct FPlayerPermissions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanPlantSeeds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanHarvestPlants;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanAdjustEnvironment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanAddNutrients;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanOperateEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanPurchaseEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanSellProduce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanManagePermissions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Permissions")
	bool bCanKickPlayers;

	FPlayerPermissions()
	{
		bCanPlantSeeds = true;
		bCanHarvestPlants = true;
		bCanAdjustEnvironment = false;
		bCanAddNutrients = false;
		bCanOperateEquipment = false;
		bCanPurchaseEquipment = false;
		bCanSellProduce = false;
		bCanManagePermissions = false;
		bCanKickPlayers = false;
	}

	static FPlayerPermissions GetOwnerPermissions()
	{
		FPlayerPermissions Permissions;
		Permissions.bCanPlantSeeds = true;
		Permissions.bCanHarvestPlants = true;
		Permissions.bCanAdjustEnvironment = true;
		Permissions.bCanAddNutrients = true;
		Permissions.bCanOperateEquipment = true;
		Permissions.bCanPurchaseEquipment = true;
		Permissions.bCanSellProduce = true;
		Permissions.bCanManagePermissions = true;
		Permissions.bCanKickPlayers = true;
		return Permissions;
	}

	static FPlayerPermissions GetManagerPermissions()
	{
		FPlayerPermissions Permissions;
		Permissions.bCanPlantSeeds = true;
		Permissions.bCanHarvestPlants = true;
		Permissions.bCanAdjustEnvironment = true;
		Permissions.bCanAddNutrients = true;
		Permissions.bCanOperateEquipment = true;
		Permissions.bCanPurchaseEquipment = true;
		Permissions.bCanSellProduce = true;
		Permissions.bCanManagePermissions = false;
		Permissions.bCanKickPlayers = false;
		return Permissions;
	}

	static FPlayerPermissions GetHelperPermissions()
	{
		FPlayerPermissions Permissions;
		Permissions.bCanPlantSeeds = true;
		Permissions.bCanHarvestPlants = true;
		Permissions.bCanAdjustEnvironment = false;
		Permissions.bCanAddNutrients = true;
		Permissions.bCanOperateEquipment = false;
		Permissions.bCanPurchaseEquipment = false;
		Permissions.bCanSellProduce = false;
		Permissions.bCanManagePermissions = false;
		Permissions.bCanKickPlayers = false;
		return Permissions;
	}

	static FPlayerPermissions GetVisitorPermissions()
	{
		FPlayerPermissions Permissions;
		Permissions.bCanPlantSeeds = false;
		Permissions.bCanHarvestPlants = false;
		Permissions.bCanAdjustEnvironment = false;
		Permissions.bCanAddNutrients = false;
		Permissions.bCanOperateEquipment = false;
		Permissions.bCanPurchaseEquipment = false;
		Permissions.bCanSellProduce = false;
		Permissions.bCanManagePermissions = false;
		Permissions.bCanKickPlayers = false;
		return Permissions;
	}
};

USTRUCT(BlueprintType)
struct FNetworkPlayerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	FString PlayerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	EPlayerRole Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	FPlayerPermissions Permissions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	bool bIsOnline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	FDateTime JoinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Player")
	int32 ContributionScore;

	FNetworkPlayerData()
	{
		PlayerName = TEXT("Unknown Player");
		PlayerID = TEXT("");
		Role = EPlayerRole::Helper;
		Permissions = FPlayerPermissions::GetHelperPermissions();
		bIsOnline = false;
		JoinTime = FDateTime::Now();
		ContributionScore = 0;
	}
};

USTRUCT(BlueprintType)
struct FNetworkActionLog
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Log")
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Log")
	ENetworkAction ActionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Log")
	FString ActionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Log")
	FDateTime Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Log")
	FVector WorldLocation;

	FNetworkActionLog()
	{
		PlayerName = TEXT("");
		ActionType = ENetworkAction::PlantSeed;
		ActionDescription = TEXT("");
		Timestamp = FDateTime::Now();
		WorldLocation = FVector::ZeroVector;
	}

	FNetworkActionLog(const FString& InPlayerName, ENetworkAction InActionType, const FString& InDescription, const FVector& InLocation = FVector::ZeroVector)
	{
		PlayerName = InPlayerName;
		ActionType = InActionType;
		ActionDescription = InDescription;
		Timestamp = FDateTime::Now();
		WorldLocation = InLocation;
	}
};

USTRUCT(BlueprintType)
struct FNetworkChatMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
	FString Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
	FDateTime Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
	bool bIsSystemMessage;

	FNetworkChatMessage()
	{
		PlayerName = TEXT("");
		Message = TEXT("");
		Timestamp = FDateTime::Now();
		bIsSystemMessage = false;
	}

	FNetworkChatMessage(const FString& InPlayerName, const FString& InMessage, bool bIsSystem = false)
	{
		PlayerName = InPlayerName;
		Message = InMessage;
		Timestamp = FDateTime::Now();
		bIsSystemMessage = bIsSystem;
	}
};

// Network functions are now defined using standard UE5 RPC syntax directly in the class headers

// Delegates for multiplayer events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, const FNetworkPlayerData&, PlayerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, const FString&, PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerRoleChanged, const FNetworkPlayerData&, PlayerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessage, const FNetworkChatMessage&, ChatMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkAction, const FNetworkActionLog&, ActionLog);