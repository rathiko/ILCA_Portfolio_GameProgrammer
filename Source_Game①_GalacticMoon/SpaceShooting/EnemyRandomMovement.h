// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnumAttackType.h"
#include "EnemyRandomMovement.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACESHOOTING_API UEnemyRandomMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyRandomMovement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RandomDirection();

	//奥行きの速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ForwardSpeed = 5000.0f;

	//左右上下の速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SideSpeed = 5000.f;

	//動く方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection = FVector(1.f, 1.f, 1.f);
	
	//動ける範囲
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinY = -10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxY = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinZ = -10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxZ = 10000;

	//動き始める距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveStartDistance = 80000;

	// 何秒に一回動くか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ChangeInterval = 2.f;

	float ElapsedTime = 0.f;

	//敵の攻撃タイプ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
	EEnemyAttackType AttackType = EEnemyAttackType::Straight;

	//攻撃の範囲を設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
	float MaxTrackingAngle = 30.0f;

	//最初の向きを設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
	FRotator InitialRotation = FRotator(0.f, 0.f, 0.f);

	//狙う際の旋回速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float HomingRotationSpeed = 5.f;	
};