// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeroGMotionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UZeroGMotionComponent::UZeroGMotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UZeroGMotionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// リスポーン時に戻すため、開始時のTransformを保存
	if (AActor* Owner = GetOwner())
	{
		InitialTransform = Owner->GetActorTransform();
	}

	Velocity = InitialVelocity;
}


// Called every frame
void UZeroGMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();

	if (!IsValid(Owner) || DeltaTime <= 0.f)
	{
		return;
	}

	// -------- 並進 --------

	// 粘性抵抗による加速度(速度に比例し、逆向きに働く)
	const FVector DragAcceleration = Velocity * (-DragRate);

	// 重力と抵抗で速度を更新してから移動
	Velocity += GravityAcceleration * DeltaTime;
	Velocity += DragAcceleration * DeltaTime;

	const FVector DeltaLocation = Velocity * DeltaTime;


	// スイープ移動：途中で何かに当たったら、その位置で止まりHitに情報が入る
	FHitResult Hit;
	Owner->AddActorWorldOffset(DeltaLocation, true, &Hit, ETeleportType::None);


	if (Hit.bBlockingHit)
	{
		ResolveCollision(Hit);
	}

	//-------- 回転 --------

	// 回転の減衰
	AngularVelocity -= AngularVelocity * (AngularDrag * DeltaTime);

	const float AngularSpeed = AngularVelocity.Size();

	if (AngularSpeed > KINDA_SMALL_NUMBER)
	{
		// 角速度の向きを回転軸、大きさ × 時間 を回転角として、1フレーム分の回転を作成
		const float AngleRadians = AngularSpeed * DeltaTime;
		const FVector RotationAxis = AngularVelocity / AngularSpeed;
		const FQuat DeltaRotation(RotationAxis, AngleRadians);

		// 角速度はワールド空間なので現在の回転の外側から掛ける(DeltaRotation * 現在)
		Owner->SetActorRotation(DeltaRotation * Owner->GetActorQuat());
	}
}

void UZeroGMotionComponent::ApplyImpulseAtPoint(const FVector& Impulse, const FVector& WorldPoint)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner) || Mass <= KINDA_SMALL_NUMBER || MomentOfInertia <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// 並進：力積 = 運動量の変化 → Δv = J / m
	Velocity += Impulse / Mass;

	// 回転：角力積 r × J = 角運動量の変化 → Δω = (r × J) / I
	const FVector LeverArm = WorldPoint - Owner->GetActorLocation();
	AngularVelocity += FVector::CrossProduct(LeverArm, Impulse) / MomentOfInertia;

	if (bDrawDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("V: %s / W: %s"), *Velocity.ToString(), *AngularVelocity.ToString());
	}
}

void UZeroGMotionComponent::ResolveCollision(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();

	// 相手も浮遊物なら、両者に作用・反作用の力積を加える
	if (HitActor)
	{
		if (UZeroGMotionComponent* OtherBody = HitActor->FindComponentByClass<UZeroGMotionComponent>())
		{
			ResolveBodyContact(OtherBody, Hit.ImpactPoint, Hit.ImpactNormal);
			return;
		}
	}

	// それ以外は静止した質量無限大の壁として扱う
	FVector OtherVelocity = FVector::ZeroVector;
	float OtherInvMass = 0.f;

	// 相手がキャラクターならその速度と質量を使用
	// 補足：動いているプレイヤーを壁として扱うと摩擦の向きが逆になってしまうため
	if (const ACharacter* OtherCharacter = Cast<ACharacter>(Hit.GetActor()))
	{
		OtherVelocity = OtherCharacter->GetVelocity();

		const float OtherMass = OtherCharacter->GetCharacterMovement()->Mass;
		if (OtherMass > KINDA_SMALL_NUMBER)
		{
			OtherInvMass = 1.f / OtherMass;
		}
	}

	ResolveContact(Hit.ImpactPoint, Hit.ImpactNormal, OtherVelocity, OtherInvMass);
}

void UZeroGMotionComponent::ResolveContact(const FVector& ContactPoint, const FVector& InNormal, const FVector& OtherVelocity, float OtherInvMass)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner) || Mass <= KINDA_SMALL_NUMBER || MomentOfInertia <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float InvMass = 1.f / Mass;

	// 検証用：衝突前のエネルギー(並進 + 回転)
	const float EnergyBefore = 0.5f * Mass * Velocity.SizeSquared() + 0.5f * MomentOfInertia * AngularVelocity.SizeSquared();

	// 相手から自分へ向く法線
	const FVector Normal = InNormal.GetSafeNormal();

	// 重心からの接触点へのベクトル
	const FVector HitArm = ContactPoint - Owner->GetActorLocation();

	// 接触点の速度 = 並進速度 + 回転による速度(ω × r)
	const FVector Vpoint = Velocity + FVector::CrossProduct(AngularVelocity, HitArm);

	// 相手に対する相対速度
	const FVector Vrel = Vpoint - OtherVelocity;

	// 相対速度の法線成分(負なら接近中、0以上なら離れていく途中)
	const float Vnormal = FVector::DotProduct(Vrel, Normal);

	if (bDrawDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("Contact: Vn=%f / OtherV=%s / InvM=%f"), Vnormal, *OtherVelocity.ToString(), OtherInvMass);
	}

	// 離れていく途中なら何もしない
	if (Vnormal >= 0.f)
	{
		return;
	}

	// -------- 法線方向の力積 --------

	// 接触点で見た動きやすさ = 有効質量の逆数
	// 1/m(並進) + 1/N(相手) + |r × n|^2 / I(回転)
	const float Denom = (InvMass + OtherInvMass) + (FVector::CrossProduct(HitArm, Normal).SizeSquared() / MomentOfInertia);

	// ゆっくりした場合は反発させない
	const float E = (-Vnormal < RestingThreshold) ? 0.f : Restitution;

	// 衝突後の法線速度が -e × Vn になるために必要な力積
	const float Jn = -(1.f + E) * Vnormal / Denom;

	// -------- 接線方向の力積 --------

	float Jt = 0.f;
	FVector Tangent = FVector::ZeroVector;

	// 相対速度から法線成分を除いた面に沿って滑っている速度
	const FVector Vtan = Vrel - Vnormal * Normal;

	if (!Vtan.IsNearlyZero())
	{
		Tangent = Vtan.GetSafeNormal();

		// 法線方向と同じ考え方で、接線方向の動きやすさを求める
		const float DenomT = InvMass + OtherInvMass + FVector::CrossProduct(HitArm, Tangent).SizeSquared() / MomentOfInertia;

		// 滑りを完全に止めるのに必要な力積
		Jt = -FVector::DotProduct(Vrel, Tangent) / DenomT;

		// 摩擦は法線方向の力積のμ倍までしか出せない
		Jt = FMath::Clamp(Jt, -Friction * Jn, Friction * Jn);
	}

	// 法線と接線の力積をまとめて接触点に一回で加える
	ApplyImpulseAtPoint(Jn * Normal + Jt * Tangent, ContactPoint);

	// 床の上でほぼ止まっていれば完全に停止させる
	if (Normal.Z > 0.7f && Velocity.Size() < SleepLinearThreshold && AngularVelocity.Size() < SleepAngularThreshold)
	{
		Velocity = FVector::ZeroVector;
		AngularVelocity = FVector::ZeroVector;
	}


	// 検証用：衝突後のエネルギー
	// 反発係数1・摩擦0なら衝突前と一致し、摩擦ありなら必ず減少
	const float EnergyAfter = 0.5f * Mass * Velocity.SizeSquared() + 0.5f * MomentOfInertia * AngularVelocity.SizeSquared();

	if (bDrawDebug) 
	{
		UE_LOG(LogTemp, Log, TEXT("EnergyBefore:%f / After: %f"), EnergyBefore, EnergyAfter);
	}
}

void UZeroGMotionComponent::ResolveBodyContact(UZeroGMotionComponent* Other,
	const FVector& ContactPoint, const FVector& InNormal)
{
	AActor* Owner = GetOwner();
	AActor* OtherOwner = Other ? Other->GetOwner() : nullptr;

	if (!IsValid(Owner) || !IsValid(OtherOwner) ||
		Mass <= KINDA_SMALL_NUMBER || MomentOfInertia <= KINDA_SMALL_NUMBER || Other->Mass <= KINDA_SMALL_NUMBER || Other->MomentOfInertia <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float InvMassA = 1.f / Mass;
	const float InvMassB = 1.f / Other->Mass;
	const float InvIA = 1.f / MomentOfInertia;
	const float InvIB = 1.f / Other->MomentOfInertia;

	const FVector Normal = InNormal.GetSafeNormal();

	// それぞれの重心から接触点へのベクトル
	const FVector RA = ContactPoint - Owner->GetActorLocation();
	const FVector RB = ContactPoint - OtherOwner->GetActorLocation();

	// それぞれの接触点の速度と、相対速度
	const FVector VA = Velocity + FVector::CrossProduct(AngularVelocity, RA);
	const FVector VB = Other->Velocity + FVector::CrossProduct(Other->AngularVelocity, RB);
	const FVector Vrel = VA - VB;

	const float Vnormal = FVector::DotProduct(Vrel, Normal);
	if (Vnormal >= 0.f)
	{
		return;
	}

	// 反発係数と摩擦係数は2つの物体の平均を使用
	const float E = 0.5f * (Restitution + Other->Restitution);
	const float Mu = 0.5f * (Friction + Other->Friction);

	// 法線方向：両者の並進と回転の動きやすさを合算
	const float Denom = InvMassA + InvMassB
		+ FVector::CrossProduct(RA, Normal).SizeSquared() * InvIA
		+ FVector::CrossProduct(RB, Normal).SizeSquared() * InvIB;
	const float Jn = -(1.f + E) * Vnormal / Denom;

	// 接線方向(摩擦)
	float Jt = 0.f;
	FVector Tangent = FVector::ZeroVector;
	const FVector Vtan = Vrel - Vnormal * Normal;
	if (!Vtan.IsNearlyZero())
	{
		Tangent = Vtan.GetSafeNormal();
		const float DenomT = InvMassA + InvMassB+ FVector::CrossProduct(RA, Tangent).SizeSquared() * InvIA+ FVector::CrossProduct(RB, Tangent).SizeSquared() * InvIB;
		Jt = -FVector::DotProduct(Vrel, Tangent) / DenomT;
		Jt = FMath::Clamp(Jt, -Mu * Jn, Mu * Jn);
	}

	// 作用・反作用:自分に +J、相手に −J を同じ接触点へ加える
	const FVector Impulse = Jn * Normal + Jt * Tangent;
	ApplyImpulseAtPoint(Impulse, ContactPoint);
	Other->ApplyImpulseAtPoint(-Impulse, ContactPoint);
}

void UZeroGMotionComponent::SetGravityEnabled(bool bEnabled)
{
	// UEの単位はcmのため、地球の重力 = -980 cm/s^2
	GravityAcceleration = bEnabled ? FVector(0.f, 0.f, -980.f) : FVector::ZeroVector;
}

void UZeroGMotionComponent::ResetBody()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// 動きの状態を初期化し、無重力に戻す
	Velocity = InitialVelocity;
	AngularVelocity = FVector::ZeroVector;
	SetGravityEnabled(false);

	// スイープせずに初期位置へ瞬間移動
	Owner->SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);
}