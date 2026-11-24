// Fill out your copyright notice in the Description page of Project Settings.


#include "DistanceCalculationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UDistanceCalculationComponent::UDistanceCalculationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDistanceCalculationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UDistanceCalculationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceLastCheck += DeltaTime;

	if (TimeSinceLastCheck >= CheckInterval)
	{
		TimeSinceLastCheck = 0.f;

		UpdateDistanceCheck();
	}
}

void UDistanceCalculationComponent::UpdateDistanceCheck()
{
	AActor* Owner = GetOwner();
	APawn* PlayerPawn =
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!Owner || !PlayerPawn)
	{
		return;
	}

	// Actor‚ÆPlayer‚ÌXÀ•W‚ðŽæ“¾
	float PlayerX = PlayerPawn->GetActorLocation().X;
	float OwnerX = Owner->GetActorLocation().X;
	DistanceX = OwnerX - PlayerX;

	// Player‚É‚ ‚é’ö“x‹ß‚Ã‚¢‚½‚ç•\Ž¦ true, ‚»‚¤‚Å‚È‚¢‚Æ‚«‚Ífalse
	bIsNearPlayer =
		(DistanceX) < VisibleDistance
		&&
		(DistanceX) > HiddenDistance;
}

