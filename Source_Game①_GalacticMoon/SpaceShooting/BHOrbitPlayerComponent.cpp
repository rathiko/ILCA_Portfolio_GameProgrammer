// Fill out your copyright notice in the Description page of Project Settings.


#include "BHOrbitPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "BlackHoleComponent.h"

// Sets default values for this component's properties
UBHOrbitPlayerComponent::UBHOrbitPlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// デフォルトの位置、速度
	Position = FVector::ZeroVector;
	Velocity = FVector::ZeroVector;
}

void UBHOrbitPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 重力コンポーネントを探す
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BHGravity"), FoundActors);

	if (FoundActors.Num() > 0)
	{
		TargetBlackHole = FoundActors[0]->FindComponentByClass <UBlackHoleComponent>();
	}

	Position = GetOwner()->GetActorLocation();
}

// Called every frame
void UBHOrbitPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// フレームレート非依存の精度を確保するため Accumulator を採用
	Accumulator += DeltaTime;

	if (TargetBlackHole)
	{
		// 相対論的運動をルンゲクッタ法を用いて数値計算
		// ルンゲクッタ法によって計算することでオイラー法と比べてより精密に計算することができ、リアルな挙動となるのが特徴
		while (Accumulator >= FixedStep)
		{
			IntegrateRK4(FixedStep);
			Accumulator -= FixedStep;
		}

	}
	else
	{
		// 重力コンポーネントが見つかっていない場合 惰性飛行をオイラー法により更新
		Position += Velocity * DeltaTime;
	}

	// 平面でのみ行動可能にし、Actorへ反映
	Position.Z = 0.f;
	Velocity.Z = 0.f;

	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorLocation(Position);
	}
}

	// ルンゲクッタ法による積分で加速度を計算
void UBHOrbitPlayerComponent::IntegrateRK4(float Dt)
{
	float Distance2 = FVector::DistSquared(Position, TargetBlackHole->GetOwner()->GetActorLocation());

	// Rsの内側に入ったら速度をゼロにして、位置更新を止める
	if (Distance2 <= FMath::Square(TargetBlackHole->Rs))
	{
		Velocity = FVector::ZeroVector;

		return;
	}
	// 現在の状態
	FVector P = Position;
	FVector V = Velocity;

	// 加速度を計算するラムダ関数（RHSの代わり）
	// 引数に渡した Position, Velocity からその瞬間の加速度を返す
	auto GetAcceleration = [&](const FVector& InPosition, const FVector& InVector) -> FVector {return TargetBlackHole ? TargetBlackHole->GetBHAcceleration(InPosition, InVector) : FVector::ZeroVector;};

	// k1
	// k1
	FVector kv1 = V;
	FVector ka1 = GetAcceleration(P, V);

	// k2
	FVector kv2 = V + ka1 * (Dt * 0.5f);
	FVector ka2 = GetAcceleration(P + kv1 * (Dt * 0.5f), kv2);

	// k3
	FVector kv3 = V + ka2 * (Dt * 0.5f);
	FVector ka3 = GetAcceleration(P + kv2 * (Dt * 0.5f), kv3);

	// k4
	FVector kv4 = V + ka3 * Dt;
	FVector ka4 = GetAcceleration(P + kv3 * Dt, kv4);

	// 位置と速度を公式通りに更新
	Position += (kv1 + 2.f * kv2 + 2.f * kv3 + kv4) * (Dt / 6.f);
	Velocity += (ka1 + 2.f * ka2 + 2.f * ka3 + ka4) * (Dt / 6.f);

}

