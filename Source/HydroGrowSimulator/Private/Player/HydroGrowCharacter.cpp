#include "Player/HydroGrowCharacter.h"
#include "Components/InteractionComponent.h"
#include "Plants/PlantActor.h"
#include "Systems/HydroponicsContainer.h"
#include "Core/HydroGrowGameInstance.h"
#include "Network/HydroGrowNetworkGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AHydroGrowCharacter::AHydroGrowCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetNetUpdateFrequency(30.0f);

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(42.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Don't rotate when the controller rotates. Let the character movement handle rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create camera root component
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(RootComponent);
	CameraRoot->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f)); // Eye height

	// Create third person camera
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraRoot);
	ThirdPersonCamera->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f)); // Behind and above character
	ThirdPersonCamera->bUsePawnControlRotation = true;

	// Configure the character mesh
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// Create interaction component
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetupAttachment(CameraRoot);

	// Initialize default values
	WalkSpeed = 500.0f;
	SprintSpeed = 800.0f;
	CrouchSpeed = 200.0f;
	DefaultWalkSpeed = WalkSpeed;

	InteractionRange = 300.0f;
	InteractionSphereRadius = 350.0f;

	PlayerLevel = 1;
	Experience = 0.0f;
	Currency = 100; // Starting currency
	MaxInventorySlots = 20;

	bIsSprinting = false;
	bInventoryOpen = false;
	CurrentInteractable = nullptr;

	PlayerName = TEXT("Player");
}

void AHydroGrowCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Set up interaction component
	if (InteractionComponent)
	{
		InteractionComponent->SetSphereRadius(InteractionSphereRadius);
		InteractionComponent->OnInteractableFound.AddDynamic(this, &AHydroGrowCharacter::OnInteractableFound);
		InteractionComponent->OnInteractableLost.AddDynamic(this, &AHydroGrowCharacter::OnInteractableLost);
	}

	// Initialize player name for networking
	if (HasAuthority())
	{
		if (APlayerController* PC = GetController<APlayerController>())
		{
			if (PC->GetPlayerState<APlayerState>())
			{
				PlayerName = PC->GetPlayerState<APlayerState>()->GetPlayerName();
			}
		}
	}
}

void AHydroGrowCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update interaction system
	if (IsLocallyControlled())
	{
		FindInteractables();
	}
}

void AHydroGrowCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHydroGrowCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHydroGrowCharacter::Look);

		// Interaction
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHydroGrowCharacter::Interact);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHydroGrowCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHydroGrowCharacter::StopSprinting);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchToggleAction, ETriggerEvent::Started, this, &AHydroGrowCharacter::ToggleCrouch);

		// Inventory
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AHydroGrowCharacter::OpenInventory);
	}
}

void AHydroGrowCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHydroGrowCharacter, PlayerName, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AHydroGrowCharacter, PlayerLevel, COND_None);
	DOREPLIFETIME_CONDITION(AHydroGrowCharacter, Experience, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHydroGrowCharacter, Currency, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHydroGrowCharacter, InventoryItems, COND_OwnerOnly);
}

void AHydroGrowCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AHydroGrowCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHydroGrowCharacter::StartSprinting()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AHydroGrowCharacter::StopSprinting()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = bIsCrouched ? CrouchSpeed : WalkSpeed;
}

void AHydroGrowCharacter::ToggleCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
		GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	}
	else
	{
		Crouch();
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		bIsSprinting = false; // Stop sprinting when crouching
	}
}

void AHydroGrowCharacter::Interact()
{
	if (CurrentInteractable)
	{
		Server_Interact(CurrentInteractable);
	}
}

void AHydroGrowCharacter::OpenInventory()
{
	bInventoryOpen = !bInventoryOpen;
	// TODO: Open inventory UI
	UE_LOG(LogTemp, Log, TEXT("Inventory toggled: %s"), bInventoryOpen ? TEXT("Open") : TEXT("Closed"));
}

bool AHydroGrowCharacter::Server_Interact_Validate(AActor* TargetActor)
{
	// Validate interaction
	if (!TargetActor)
	{
		return false;
	}

	// Check distance
	float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
	if (Distance > InteractionRange * 1.5f) // Allow some tolerance for network lag
	{
		return false;
	}

	return true;
}

void AHydroGrowCharacter::Server_Interact_Implementation(AActor* TargetActor)
{
	if (CanInteractWith(TargetActor))
	{
		InteractWithActor(TargetActor);
	}
}

void AHydroGrowCharacter::SetPlayerName(const FString& NewName)
{
	if (HasAuthority())
	{
		PlayerName = NewName;
	}
}

void AHydroGrowCharacter::AddExperience(float Amount)
{
	if (HasAuthority())
	{
		Experience += Amount;
		CheckLevelUp();
	}
}

void AHydroGrowCharacter::AddCurrency(int32 Amount)
{
	if (HasAuthority())
	{
		Currency += Amount;
		Currency = FMath::Max(0, Currency); // Ensure non-negative
	}
}

bool AHydroGrowCharacter::CanAfford(int32 Cost) const
{
	return Currency >= Cost;
}

bool AHydroGrowCharacter::SpendCurrency(int32 Amount)
{
	if (HasAuthority() && CanAfford(Amount))
	{
		Currency -= Amount;
		return true;
	}
	return false;
}

void AHydroGrowCharacter::FindInteractables()
{
	if (!InteractionComponent)
	{
		return;
	}

	// Get camera location and forward vector for line trace
	FVector CameraLocation = ThirdPersonCamera->GetComponentLocation();
	FVector CameraForward = ThirdPersonCamera->GetForwardVector();
	FVector TraceEnd = CameraLocation + (CameraForward * InteractionRange);

	// Line trace for interactables
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	AActor* NewInteractable = nullptr;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (CanInteractWith(HitActor))
			{
				NewInteractable = HitActor;
			}
		}
	}

	// Update current interactable
	if (NewInteractable != CurrentInteractable)
	{
		if (CurrentInteractable)
		{
			OnInteractionLost.Broadcast();
		}

		CurrentInteractable = NewInteractable;

		if (CurrentInteractable)
		{
			OnInteractionFound.Broadcast(CurrentInteractable);
		}
	}
}

bool AHydroGrowCharacter::CanInteractWith(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	// Check if actor implements IInteractable interface
	if (Actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		return IInteractable::Execute_CanInteract(Actor, const_cast<AHydroGrowCharacter*>(this));
	}

	// Check for specific actor types
	if (Actor->IsA<AHydroponicsContainer>() || Actor->IsA<APlantActor>())
	{
		return true;
	}

	return false;
}

void AHydroGrowCharacter::InteractWithActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	// Handle interface-based interaction
	if (Actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		IInteractable::Execute_OnInteract(Actor, this);
		return;
	}

	// Handle specific actor types
	if (AHydroponicsContainer* Container = Cast<AHydroponicsContainer>(Actor))
	{
		InteractWithContainer(Container);
	}
	else if (APlantActor* Plant = Cast<APlantActor>(Actor))
	{
		InteractWithPlant(Plant);
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s interacted with %s"), *PlayerName, *Actor->GetName());
}

void AHydroGrowCharacter::InteractWithContainer(AHydroponicsContainer* Container)
{
	if (!Container)
	{
		return;
	}

	// Get player ID for permission checking
	FString PlayerID = PlayerName;
	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
		{
			PlayerID = FString::FromInt(PS->GetPlayerId()) + "_" + PS->GetPlayerName();
		}
	}

	// Example interactions with container
	UE_LOG(LogTemp, Warning, TEXT("Interacting with container: %s"), *Container->GetName());
	
	// TODO: Open container interaction UI
	// This would show options like:
	// - Plant seeds
	// - Adjust pH
	// - Add nutrients
	// - Check status
}

void AHydroGrowCharacter::InteractWithPlant(APlantActor* Plant)
{
	if (!Plant)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Interacting with plant: %s"), *Plant->GetName());
	
	// Example plant interactions
	if (Plant->CanHarvestPlant())
	{
		// Harvest the plant
		Plant->HarvestPlant(PlayerName);
		AddExperience(10.0f); // Gain experience for harvesting
	}
	
	// TODO: Open plant interaction UI
	// This would show options like:
	// - Water plant
	// - Add nutrients
	// - Check health
	// - Harvest (if ready)
}

bool AHydroGrowCharacter::HasItem(FName ItemID) const
{
	return InventoryItems.Contains(ItemID);
}

bool AHydroGrowCharacter::AddItem(FName ItemID)
{
	if (HasAuthority())
	{
		if (InventoryItems.Num() < MaxInventorySlots)
		{
			InventoryItems.Add(ItemID);
			return true;
		}
	}
	return false;
}

bool AHydroGrowCharacter::RemoveItem(FName ItemID)
{
	if (HasAuthority())
	{
		return InventoryItems.RemoveSingle(ItemID) > 0;
	}
	return false;
}

int32 AHydroGrowCharacter::GetItemCount(FName ItemID) const
{
	int32 Count = 0;
	for (const FName& Item : InventoryItems)
	{
		if (Item == ItemID)
		{
			Count++;
		}
	}
	return Count;
}

void AHydroGrowCharacter::CheckLevelUp()
{
	float RequiredXP = GetExperienceForLevel(PlayerLevel + 1);
	if (Experience >= RequiredXP)
	{
		PlayerLevel++;
		Experience -= RequiredXP;
		
		// Add currency reward for leveling up
		AddCurrency(50 * PlayerLevel);
		
		UE_LOG(LogTemp, Warning, TEXT("Player %s leveled up to level %d!"), *PlayerName, PlayerLevel);
		
		// Check for another level up
		CheckLevelUp();
	}
}

float AHydroGrowCharacter::GetExperienceForLevel(int32 Level) const
{
	// Exponential XP curve: 100 * Level^1.5
	return 100.0f * FMath::Pow(static_cast<float>(Level), 1.5f);
}

void AHydroGrowCharacter::OnRep_PlayerLevel()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s is now level %d"), *PlayerName, PlayerLevel);
}

void AHydroGrowCharacter::OnRep_Currency()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s currency updated: %d"), *PlayerName, Currency);
}

void AHydroGrowCharacter::OnRep_Experience()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s experience updated: %.1f"), *PlayerName, Experience);
}

void AHydroGrowCharacter::OnInteractableFound(AActor* InteractableActor)
{
	CurrentInteractable = InteractableActor;
	OnInteractionFound.Broadcast(InteractableActor);
}

void AHydroGrowCharacter::OnInteractableLost(AActor* Actor)
{
	CurrentInteractable = nullptr;
	OnInteractionLost.Broadcast();
}