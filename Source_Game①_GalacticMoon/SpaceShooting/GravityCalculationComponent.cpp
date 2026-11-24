// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityCalculationComponent.h"
#include "Kismet/GameplayStatics.h"

UGravityCalculationComponent::UGravityCalculationComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UGravityCalculationComponent::BeginPlay()
{
	Super::BeginPlay();

	//レベル内にある「Planet」タグつけたアクターを取得
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Planet"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		PlanetActor = FoundActors[0];
	}

}

// Called every frame
void UGravityCalculationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlanetActor) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector Direction = PlanetActor->GetActorLocation() - Owner->GetActorLocation();

	// 距離の2乗で割る計算
	float DistanceSq = Direction.SizeSquared();
	Direction.Normalize();

	// 万有引力の計算(1.0f はゼロ除算防止)
	GravityAccel = Direction * (GravityStrength / FMath::Max(DistanceSq, 1.0f));
}

