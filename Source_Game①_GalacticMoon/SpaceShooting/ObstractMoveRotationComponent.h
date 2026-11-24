// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObstractMoveRotationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHOOTING_API UObstractMoveRotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObstractMoveRotationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate")
	FRotator RotationSpeed = FRotator(0.0f, 90.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
	FVector MoveDirection = FVector(-1.f, 0.0f, 0.0f);

};