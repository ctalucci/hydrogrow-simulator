#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "HydroGrowCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class AHydroponicsContainer;
class APlantActor;
class UInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFound, AActor*, InteractableActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionLost);

UCLASS(config=Game, BlueprintType, meta=(ShortTooltip="A character for our hyrdoponics game."))
class HYDROGROWSIMULATOR_API AHydroGrowCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHydroGrowCharacter();

	// Character functionality
	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetPlayerName(const FString& NewName);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InventoryAction;

	// Camera and Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* CameraRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractionComponent* InteractionComponent;

	// Character Stats (Replicated)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FString PlayerName;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
	int32 PlayerLevel;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
	float Experience;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
	int32 Currency;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed;

	// Interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionSphereRadius;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* CurrentInteractable;

	// Input Actions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprinting();
	void StopSprinting();
	void ToggleCrouch();
	void Interact();
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void AddExperience(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void AddCurrency(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool CanAfford(int32 Cost) const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool SpendCurrency(int32 Amount);

	// Interaction system
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void FindInteractables();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CanInteractWith(AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InteractWithActor(AActor* Actor);

	// Network functions for interaction
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact(AActor* TargetActor);
	bool Server_Interact_Validate(AActor* TargetActor);
	void Server_Interact_Implementation(AActor* TargetActor);

	// Plant/Container specific interactions
	UFUNCTION(BlueprintCallable, Category = "Gardening")
	void InteractWithContainer(AHydroponicsContainer* Container);

	UFUNCTION(BlueprintCallable, Category = "Gardening")
	void InteractWithPlant(APlantActor* Plant);

	// Inventory and tools
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FName> InventoryItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxInventorySlots;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;
	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFound OnInteractionFound;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionLost OnInteractionLost;

	// Getters
	UFUNCTION(BlueprintPure, Category = "Character")
	FString GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintPure, Category = "Character")
	int32 GetPlayerLevel() const { return PlayerLevel; }

	UFUNCTION(BlueprintPure, Category = "Character")
	float GetExperience() const { return Experience; }

	UFUNCTION(BlueprintPure, Category = "Character")
	int32 GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetCurrentInteractable() const { return CurrentInteractable; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	UCameraComponent* GetThirdPersonCameraComponent() const { return ThirdPersonCamera; }

private:
	// Internal state
	bool bIsSprinting;
	bool bInventoryOpen;
	float DefaultWalkSpeed;

	// Experience system
	void CheckLevelUp();
	float GetExperienceForLevel(int32 Level) const;

	// Interaction callbacks
	UFUNCTION()
	void OnInteractableFound(AActor* InteractableActor);

	UFUNCTION()
	void OnInteractableLost(AActor* Actor);

	// Replication callbacks
	UFUNCTION()
	void OnRep_PlayerLevel();

	UFUNCTION()
	void OnRep_Currency();

	UFUNCTION()
	void OnRep_Experience();
};