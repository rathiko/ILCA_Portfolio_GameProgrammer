// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "E_EnemyMoveType.h"
#include "EnemyMoveComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACESHOOTING_API UEnemyMoveComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyMoveComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	// 敵の挙動管理
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	E_EnemyMoveType MoveType = E_EnemyMoveType::Straight;

	// 動く速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 1000.f;

	// 動く方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MoveDirection = FVector(-1.f, 0.f, 0.f);

	// Randomの上下移動幅
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveRange = 300.f;

	// Randomの上下移動周期
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveFrequency = 2.f;

	// Cycleの円軌道半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CycleRadius = 4000.f;

	// Cycleの円軌道速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CycleSpeed = 2.f;

	// 移動開始(AppearComponentのOnAppearFinishedから呼ばれる)
	UFUNCTION(BlueprintCallable)
	void StartMove();

	// 移動停止(Retreat時・やられたときに呼ばれる)
	UFUNCTION(BlueprintCallable)
	void StopMove();

	// 自動回転停止(Battle移行時にAppearComponentから呼ばれる)
	UFUNCTION(BlueprintCallable)
	void StopAutoRotation();

private:
	AActor* OwnerActor;

	// 移動処理のON/OFFフラグ
	bool bCanMove = false;

	// Sin・Cos計算用の経過時間
	float Time = 0.f;

	// Random・Cycleの基準位置(BeginPlayで初期化)
	FVector StartLocation;

	// 自動回転のON/OFFフラグ(Appear中のみtrue)
	bool bAutoRotation = false;
};