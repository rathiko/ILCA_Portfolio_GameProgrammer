// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlackHoleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHOOTING_API UBlackHoleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlackHoleComponent();

	FVector BHGravityAccel;

	// ブラックホールの影響半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float InfluenceRadius = 2.0e8f;

	//ブラックホールの質量M
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float M = 80000.f / 2.f;

	// シュワルツシルト半径
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackHole")
	float Rs;

	// 重力の強さ係数A
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float A = 1e12f;

	// 光速
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float c = 3.0e8f;

	// 与えられた位置・速度に対する相対論補正
	UFUNCTION(BlueprintCallable, Category = "BlackHole")
	FVector GetBHAcceleration(const FVector& WorldPosition, const FVector& WorldVelocity) const;

	// 影響範囲内かどうかの判定
	UFUNCTION(BlueprintCallable, Category = "BlackHole")
	bool IsWithinInfluence(const FVector& WorldPosition) const;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
