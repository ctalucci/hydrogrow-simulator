#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AHydroGrowCharacter;

USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bRequiresItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FName RequiredItemID;

	FInteractionData()
	{
		InteractionText = TEXT("Interact");
		InteractionDuration = 0.0f;
		bRequiresItem = false;
		RequiredItemID = NAME_None;
	}
};

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class HYDROGROWSIMULATOR_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	FInteractionData GetInteractionData() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AHydroGrowCharacter* Character) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AHydroGrowCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionStart(AHydroGrowCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionEnd(AHydroGrowCharacter* Character);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HYDROGROWSIMULATOR_API UInteractionComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Interaction settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionConeAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bRequireLineOfSight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TArray<TEnumAsByte<EObjectTypeQuery>> InteractionObjectTypes;

	// Current state
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TArray<AActor*> NearbyInteractables;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* BestInteractable;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bIsInteracting;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	float InteractionProgress;

public:
	// Public interface
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StartInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StopInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetBestInteractable() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CanInteractWithActor(AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FInteractionData GetInteractionDataForActor(AActor* Actor) const;

	// Delegates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFound, AActor*, Actor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableLost, AActor*, Actor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionStarted, AActor*, Actor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionCompleted, AActor*, Actor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionCancelled);

	UPROPERTY(BlueprintAssignable)
	FOnInteractableFound OnInteractableFound;

	UPROPERTY(BlueprintAssignable)
	FOnInteractableLost OnInteractableLost;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionStarted OnInteractionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionCompleted OnInteractionCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionCancelled OnInteractionCancelled;

private:
	AHydroGrowCharacter* OwnerCharacter;
	FTimerHandle InteractionTimerHandle;
	AActor* CurrentInteractionTarget;
	FInteractionData CurrentInteractionData;

	void FindInteractables();
	AActor* FindBestInteractable();
	bool IsActorInInteractionCone(AActor* Actor) const;
	bool HasLineOfSightToActor(AActor* Actor) const;
	void CompleteInteraction();
	void UpdateInteractionProgress();
};