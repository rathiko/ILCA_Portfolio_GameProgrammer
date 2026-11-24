// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GravityPawnMovement.generated.h"

class UGravityCalculationComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACESHOOTING_API UGravityPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UGravityPawnMovement();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float k = 2.0f;

	UPROPERTY(VisibleAnywhere)
	FVector GravityVelocity;

	UPROPERTY()
	TObjectPtr<UGravityCalculationComponent> GravityComponent;

protected:

	virtual void BeginPlay() override;
};
