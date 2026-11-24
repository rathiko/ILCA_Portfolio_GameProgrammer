// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstractMoveRotationComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UObstractMoveRotationComponent::UObstractMoveRotationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UObstractMoveRotationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UObstractMoveRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner) return;

	//ˆÚ“®
	FVector DeltaLocation = MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime;
	Owner->AddActorWorldOffset(DeltaLocation, true);

	//‰ñ“]
	FRotator DeltaRotation = RotationSpeed * DeltaTime;
	Owner->AddActorLocalRotation(DeltaRotation, true);
}

