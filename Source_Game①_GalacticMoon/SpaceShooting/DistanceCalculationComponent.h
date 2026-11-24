// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DistanceCalculationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHOOTING_API UDistanceCalculationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDistanceCalculationComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// BPで参照するbool変数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	bool bIsNearPlayer = false;

	// 表示距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float DistanceX;

	// 表示距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float VisibleDistance = 90000.f;

	// 非表示距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float HiddenDistance = -4000.f;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	// 計算間隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float CheckInterval = 0.1f;

private:

	float TimeSinceLastCheck = 0.f;

	void UpdateDistanceCheck();
		
};
