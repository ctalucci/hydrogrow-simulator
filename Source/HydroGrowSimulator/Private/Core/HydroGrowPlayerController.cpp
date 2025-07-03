#include "Core/HydroGrowPlayerController.h"
#include "Plants/PlantActor.h"
#include "Systems/HydroponicsContainer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

AHydroGrowPlayerController::AHydroGrowPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	// Default camera settings
	CameraRotationSpeed = 2.0f;
	CameraZoomSpeed = 100.0f;
	MinZoomDistance = 200.0f;
	MaxZoomDistance = 2000.0f;
	CurrentCameraDistance = 800.0f;
	
	CameraRotation = FRotator(-45.0f, 0.0f, 0.0f);
}

void AHydroGrowPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AHydroGrowPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		if (CameraRotateAction)
		{
			EnhancedInputComponent->BindAction(CameraRotateAction, ETriggerEvent::Triggered, this, &AHydroGrowPlayerController::CameraRotate);
		}
		
		if (CameraZoomAction)
		{
			EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AHydroGrowPlayerController::CameraZoom);
		}
		
		if (SelectObjectAction)
		{
			EnhancedInputComponent->BindAction(SelectObjectAction, ETriggerEvent::Started, this, &AHydroGrowPlayerController::SelectObject);
		}
		
		if (ContextMenuAction)
		{
			EnhancedInputComponent->BindAction(ContextMenuAction, ETriggerEvent::Started, this, &AHydroGrowPlayerController::OpenContextMenu);
		}
		
		if (QuickHarvestAction)
		{
			EnhancedInputComponent->BindAction(QuickHarvestAction, ETriggerEvent::Started, this, &AHydroGrowPlayerController::QuickHarvest);
		}
		
		if (ToggleSpeedAction)
		{
			EnhancedInputComponent->BindAction(ToggleSpeedAction, ETriggerEvent::Started, this, &AHydroGrowPlayerController::ToggleSpeed);
		}
		
		if (OpenInventoryAction)
		{
			EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AHydroGrowPlayerController::OpenInventory);
		}
	}
}

void AHydroGrowPlayerController::SetCameraTarget(AActor* NewTarget)
{
	CameraTarget = NewTarget;
	UpdateCameraPosition();
}

void AHydroGrowPlayerController::SelectPlant(APlantActor* Plant)
{
	SelectedPlant = Plant;
	SelectedContainer = nullptr;
	OnPlantSelected.Broadcast(Plant);
	
	if (Plant)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected plant: %s"), *Plant->GetName());
	}
}

void AHydroGrowPlayerController::SelectContainer(AHydroponicsContainer* Container)
{
	SelectedContainer = Container;
	SelectedPlant = nullptr;
	OnContainerSelected.Broadcast(Container);
	
	if (Container)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected container: %s"), *Container->GetName());
	}
}

void AHydroGrowPlayerController::ToggleGameSpeed()
{
	// This will be implemented when TimeManager is created
	UE_LOG(LogTemp, Warning, TEXT("Toggle game speed"));
}

void AHydroGrowPlayerController::OpenPlantInfoPanel()
{
	if (SelectedPlant)
	{
		UE_LOG(LogTemp, Warning, TEXT("Opening plant info panel"));
		// UI implementation will be added later
	}
}

void AHydroGrowPlayerController::OpenShopPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("Opening shop panel"));
	// UI implementation will be added later
}

void AHydroGrowPlayerController::OpenInventoryPanel()
{
	UE_LOG(LogTemp, Warning, TEXT("Opening inventory panel"));
	// UI implementation will be added later
}

void AHydroGrowPlayerController::CameraRotate(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if (GetPawn())
	{
		// Rotate camera around target
		CameraRotation.Yaw += LookAxisVector.X * CameraRotationSpeed;
		CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch + LookAxisVector.Y * CameraRotationSpeed, -80.0f, 0.0f);
		
		UpdateCameraPosition();
	}
}

void AHydroGrowPlayerController::CameraZoom(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();
	
	CurrentCameraDistance = FMath::Clamp(CurrentCameraDistance + ZoomValue * CameraZoomSpeed, MinZoomDistance, MaxZoomDistance);
	UpdateCameraPosition();
}

void AHydroGrowPlayerController::SelectObject(const FInputActionValue& Value)
{
	AActor* HitActor = GetActorUnderCursor();
	
	if (APlantActor* Plant = Cast<APlantActor>(HitActor))
	{
		SelectPlant(Plant);
	}
	else if (AHydroponicsContainer* Container = Cast<AHydroponicsContainer>(HitActor))
	{
		SelectContainer(Container);
	}
	else
	{
		// Deselect
		SelectPlant(nullptr);
		SelectContainer(nullptr);
	}
}

void AHydroGrowPlayerController::OpenContextMenu(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Opening context menu"));
	// Context menu implementation will be added later
}

void AHydroGrowPlayerController::QuickHarvest(const FInputActionValue& Value)
{
	if (SelectedPlant)
	{
		// Quick harvest implementation will be added when PlantActor is complete
		UE_LOG(LogTemp, Warning, TEXT("Quick harvest"));
	}
}

void AHydroGrowPlayerController::ToggleSpeed(const FInputActionValue& Value)
{
	ToggleGameSpeed();
}

void AHydroGrowPlayerController::OpenInventory(const FInputActionValue& Value)
{
	OpenInventoryPanel();
}

void AHydroGrowPlayerController::UpdateCameraPosition()
{
	if (!GetPawn()) return;
	
	FVector TargetLocation = CameraTarget ? CameraTarget->GetActorLocation() : FVector::ZeroVector;
	
	// Calculate camera position based on rotation and distance
	FVector CameraOffset = CameraRotation.Vector() * CurrentCameraDistance;
	FVector CameraLocation = TargetLocation - CameraOffset;
	
	GetPawn()->SetActorLocation(CameraLocation);
	GetPawn()->SetActorRotation(CameraRotation);
}

AActor* AHydroGrowPlayerController::GetActorUnderCursor() const
{
	FHitResult HitResult;
	bool bHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);
	
	if (bHit)
	{
		return HitResult.GetActor();
	}
	
	return nullptr;
}