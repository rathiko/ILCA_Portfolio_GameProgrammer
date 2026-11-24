// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "GravityPawnMovement.h"
#include "GravityCalculationComponent.h"

UGravityPawnMovement::UGravityPawnMovement()
{

	PrimaryComponentTick.bCanEverTick = true;

	GravityVelocity = FVector::ZeroVector;
}

void UGravityPawnMovement::BeginPlay()
{
	Super::BeginPlay();

	// 重力コンポーネントを探す
	GravityComponent = GetOwner()->FindComponentByClass<UGravityCalculationComponent>();
}

void UGravityPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	if (GravityComponent)
	{
		// 親の移動計算が行われる前に Velocity に加算
		GravityVelocity += GravityComponent->GravityAccel * DeltaTime;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UpdatedComponent != nullptr)
	{


		FHitResult Hit;

		SafeMoveUpdatedComponent(GravityVelocity * DeltaTime, UpdatedComponent->GetComponentQuat(), true, Hit);

	}

	GravityVelocity *= FMath::Exp(-k * DeltaTime);
}

