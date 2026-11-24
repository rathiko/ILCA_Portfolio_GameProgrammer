// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlackHoleComponent.h"
#include "BHOrbitPlayerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHOOTING_API UBHOrbitPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBHOrbitPlayerComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 現在の位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RK4")
	FVector Position;

	// 現在の速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RK4")
	FVector Velocity;

	// 固定時間ステップ
	UPROPERTY(EditAnywhere, Category = "RK4")
	float FixedStep = 0.0025f;

	// 内部時間蓄積
	float Accumulator = 0.f;

protected:
	// ルンゲクッタ法によるステップ積分
	void IntegrateRK4(float Dt);
	
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UBlackHoleComponent* TargetBlackHole = nullptr;
};
