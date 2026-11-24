// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "BlackHoleComponent.h"

// Sets default values for this component's properties
UBlackHoleComponent::UBlackHoleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true; 
}

	//影響範囲チェック
	bool UBlackHoleComponent::IsWithinInfluence(const FVector & WorldPosition) const
	{
		if (!GetOwner()) return false;

		const float Distance2 = FVector::DistSquared(WorldPosition, GetOwner()->GetActorLocation());

		return Distance2 <= InfluenceRadius * InfluenceRadius;

	}

	FVector UBlackHoleComponent::GetBHAcceleration(const FVector & WorldPosition, const FVector & WorldVelocity) const
	{
		// ブラックホール中心からの位置ベクトル
		const FVector pVector = WorldPosition - GetOwner()->GetActorLocation();

		// 2D (XY平面) で扱う
		const float x = pVector.X;
		const float y = pVector.Y;
		const float vx = WorldVelocity.X;
		const float vy = WorldVelocity.Y;

		// 距離 rho = sqrt(x^2 + y^2)
		const float rho2_raw= x * x + y * y;

		//分母が0に近い場合
		if (rho2_raw < 10.f)
		{
			return FVector::ZeroVector;
		}

		const float rho2 = FMath::Max(rho2_raw, 10.f);

		const float rho = FMath::Sqrt(rho2);

		// 角運動量に対応する量 L = x*v(y) - y*v(x)
		const float l = x * vy - y * vx;

		// B = 3 * L^2 /(c * c)
		const float B = 3.0f * l * l/ (c * c);

		// 相対論補正項 (1 + B / rho^2)
		const float correction = 1.f + B / rho2;

		// 各成分の加速度
		const float ax = -A * x / (rho2 * rho) * correction;
		const float ay = -A * y / (rho2 * rho) * correction;

		return FVector(ax, ay, 0.0f);

}

// Called when the game starts
void UBlackHoleComponent::BeginPlay()
{
	Super::BeginPlay();
	// 「BHGravity」タグが付与されたアクターを取得
	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BHGravity"), FoundActor);
	
	//シュワルツシルト半径を自然単位系によって計算
	Rs = 2.f * M;
	
}

// Called every frame
void UBlackHoleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner) return;

}

