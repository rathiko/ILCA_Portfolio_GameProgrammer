// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityCalculationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHOOTING_API UGravityCalculationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	AActor* PlanetActor;

	// èdóÕÇÃã≠Ç≥
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float GravityStrength = 150000000.f;

	// èdóÕâ¡ë¨ìx
	FVector GravityAccel;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UGravityCalculationComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};