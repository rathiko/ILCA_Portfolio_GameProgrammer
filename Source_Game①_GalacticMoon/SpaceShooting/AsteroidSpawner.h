// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/RandomStream.h"
#include "AsteroidSpawner.generated.h"

UCLASS()
class SPACESHOOTING_API AAsteroidSpawner : public AActor
{
	GENERATED_BODY()

public:

	AAsteroidSpawner();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	// ISM
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* AsteroidISM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* AsteroidMesh;

	// 生成設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	int32 InstanceCount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	FVector SpawnRange = FVector(1000.f, 5000.f, 1000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	float MinScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	float MaxScale = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	int32 RandomSeed = 12345;

	bool bNeedRenderUpdate = false;

	FRandomStream RandomStream;

	// 移動
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	float ScrollSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	TArray<FRotator> RotationSpeeds;

	// 表示距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	float VisibleDistance = 160000.f;

	// 保存用
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asteroid")
	TArray<FTransform> InitialTransforms;

	UPROPERTY()
	TArray<bool> bIsVisibleArray;

	
	// 非表示位置
	FVector HiddenLocation = FVector(-100000000.f);

	// Player保存
	UPROPERTY()
	APawn* CachedPlayerPawn;

	// 距離チェック更新
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asteroid")
	float DistanceCheckInterval = 0.2f;

	float DistanceCheckAccumulator = 0.f;

	// 関数
	UFUNCTION(BlueprintCallable)
	void ResetAsteroids();

	void SetAsteroidVisible(int32 Index,bool bVisible);

	void UpdateAsteroidDistanceVisibility(const FVector& PlayerLocation);
};