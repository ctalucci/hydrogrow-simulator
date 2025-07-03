#include "Components/InteractionComponent.h"
#include "Player/HydroGrowCharacter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// Set default values
	InteractionRange = 300.0f;
	InteractionConeAngle = 60.0f;
	bRequireLineOfSight = true;
	
	// Set up collision
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	// Initialize state
	BestInteractable = nullptr;
	bIsInteracting = false;
	InteractionProgress = 0.0f;
	CurrentInteractionTarget = nullptr;
	
	// Set default object types to detect
	InteractionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	InteractionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<AHydroGrowCharacter>(GetOwner());
	SetSphereRadius(InteractionRange * 1.2f); // Slightly larger than interaction range
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
	{
		FindInteractables();
		
		AActor* NewBest = FindBestInteractable();
		if (NewBest != BestInteractable)
		{
			if (BestInteractable)
			{
				OnInteractableLost.Broadcast(BestInteractable);
			}
			
			BestInteractable = NewBest;
			
			if (BestInteractable)
			{
				OnInteractableFound.Broadcast(BestInteractable);
			}
		}
	}
	
	// Update interaction progress if interacting
	if (bIsInteracting)
	{
		UpdateInteractionProgress();
	}
}

void UInteractionComponent::StartInteraction()
{
	if (!BestInteractable || bIsInteracting)
	{
		return;
	}
	
	// Get interaction data
	CurrentInteractionData = GetInteractionDataForActor(BestInteractable);
	CurrentInteractionTarget = BestInteractable;
	
	// Check if character has required item
	if (CurrentInteractionData.bRequiresItem && OwnerCharacter)
	{
		if (!OwnerCharacter->HasItem(CurrentInteractionData.RequiredItemID))
		{
			UE_LOG(LogTemp, Warning, TEXT("Interaction requires item: %s"), *CurrentInteractionData.RequiredItemID.ToString());
			return;
		}
	}
	
	bIsInteracting = true;
	InteractionProgress = 0.0f;
	
	OnInteractionStarted.Broadcast(CurrentInteractionTarget);
	
	// Handle interface interaction start
	if (CurrentInteractionTarget->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		IInteractable::Execute_OnInteractionStart(CurrentInteractionTarget, OwnerCharacter);
	}
	
	// If it's an instant interaction, complete immediately
	if (CurrentInteractionData.InteractionDuration <= 0.0f)
	{
		CompleteInteraction();
	}
	else
	{
		// Set timer for interaction completion
		GetWorld()->GetTimerManager().SetTimer(
			InteractionTimerHandle,
			this,
			&UInteractionComponent::CompleteInteraction,
			CurrentInteractionData.InteractionDuration,
			false
		);
	}
}

void UInteractionComponent::StopInteraction()
{
	if (!bIsInteracting)
	{
		return;
	}
	
	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(InteractionTimerHandle);
	
	// Handle interface interaction end
	if (CurrentInteractionTarget && CurrentInteractionTarget->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		IInteractable::Execute_OnInteractionEnd(CurrentInteractionTarget, OwnerCharacter);
	}
	
	bIsInteracting = false;
	InteractionProgress = 0.0f;
	CurrentInteractionTarget = nullptr;
	
	OnInteractionCancelled.Broadcast();
}

AActor* UInteractionComponent::GetBestInteractable() const
{
	return BestInteractable;
}

bool UInteractionComponent::CanInteractWithActor(AActor* Actor) const
{
	if (!Actor || !OwnerCharacter)
	{
		return false;
	}
	
	// Check if actor implements IInteractable interface
	if (Actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		return IInteractable::Execute_CanInteract(Actor, OwnerCharacter);
	}
	
	// Default to true for non-interface actors
	return true;
}

FInteractionData UInteractionComponent::GetInteractionDataForActor(AActor* Actor) const
{
	if (!Actor)
	{
		return FInteractionData();
	}
	
	// Check if actor implements IInteractable interface
	if (Actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		return IInteractable::Execute_GetInteractionData(Actor);
	}
	
	// Return default interaction data
	FInteractionData DefaultData;
	DefaultData.InteractionText = FString::Printf(TEXT("Interact with %s"), *Actor->GetName());
	return DefaultData;
}

void UInteractionComponent::FindInteractables()
{
	if (!OwnerCharacter)
	{
		return;
	}
	
	NearbyInteractables.Empty();
	
	// Get overlapping actors
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor != OwnerCharacter && CanInteractWithActor(Actor))
		{
			// Check if actor is in interaction cone
			if (IsActorInInteractionCone(Actor))
			{
				// Check line of sight if required
				if (!bRequireLineOfSight || HasLineOfSightToActor(Actor))
				{
					NearbyInteractables.Add(Actor);
				}
			}
		}
	}
}

AActor* UInteractionComponent::FindBestInteractable()
{
	if (NearbyInteractables.IsEmpty())
	{
		return nullptr;
	}
	
	if (!OwnerCharacter)
	{
		return nullptr;
	}
	
	AActor* Best = nullptr;
	float BestScore = -1.0f;
	
	FVector CharacterLocation = OwnerCharacter->GetActorLocation();
	FVector CameraForward = OwnerCharacter->GetFirstPersonCameraComponent()->GetForwardVector();
	
	for (AActor* Actor : NearbyInteractables)
	{
		if (!Actor)
		{
			continue;
		}
		
		FVector ToActor = (Actor->GetActorLocation() - CharacterLocation).GetSafeNormal();
		float Distance = FVector::Dist(CharacterLocation, Actor->GetActorLocation());
		
		// Calculate dot product (how much the actor is in front of player)
		float DotProduct = FVector::DotProduct(CameraForward, ToActor);
		
		// Score based on distance and angle (closer and more centered = higher score)
		float Score = DotProduct / (1.0f + Distance * 0.01f);
		
		if (Score > BestScore)
		{
			BestScore = Score;
			Best = Actor;
		}
	}
	
	return Best;
}

bool UInteractionComponent::IsActorInInteractionCone(AActor* Actor) const
{
	if (!Actor || !OwnerCharacter)
	{
		return false;
	}
	
	FVector CharacterLocation = OwnerCharacter->GetActorLocation();
	FVector CameraForward = OwnerCharacter->GetFirstPersonCameraComponent()->GetForwardVector();
	FVector ToActor = (Actor->GetActorLocation() - CharacterLocation).GetSafeNormal();
	
	float DotProduct = FVector::DotProduct(CameraForward, ToActor);
	float ConeAngleRadians = FMath::DegreesToRadians(InteractionConeAngle * 0.5f);
	float CosAngle = FMath::Cos(ConeAngleRadians);
	
	return DotProduct >= CosAngle;
}

bool UInteractionComponent::HasLineOfSightToActor(AActor* Actor) const
{
	if (!Actor || !OwnerCharacter)
	{
		return false;
	}
	
	FVector StartLocation = OwnerCharacter->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector EndLocation = Actor->GetActorLocation();
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.AddIgnoredActor(Actor);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);
	
	return !bHit; // If no hit, we have line of sight
}

void UInteractionComponent::CompleteInteraction()
{
	if (!bIsInteracting || !CurrentInteractionTarget)
	{
		return;
	}
	
	// Handle interface interaction
	if (CurrentInteractionTarget->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		IInteractable::Execute_OnInteract(CurrentInteractionTarget, OwnerCharacter);
	}
	
	OnInteractionCompleted.Broadcast(CurrentInteractionTarget);
	
	// Reset interaction state
	bIsInteracting = false;
	InteractionProgress = 0.0f;
	CurrentInteractionTarget = nullptr;
}

void UInteractionComponent::UpdateInteractionProgress()
{
	if (!bIsInteracting || CurrentInteractionData.InteractionDuration <= 0.0f)
	{
		return;
	}
	
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(InteractionTimerHandle);
	InteractionProgress = FMath::Clamp(ElapsedTime / CurrentInteractionData.InteractionDuration, 0.0f, 1.0f);
}