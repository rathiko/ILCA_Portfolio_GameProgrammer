// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZeroGMotionComponent.generated.h"

/**
 * 無重力空間での剛体の運動を自前で計算するコンポーネント。
 *
 * - 並進:速度に重力と粘性抵抗を加え、スイープ移動で衝突を検出
 * - 回転:角速度をクォータニオンで積分(ワールド空間)
 * - 衝突:接触点への力積(法線方向+クーロン摩擦)で、並進と回転を同時に更新
 *
 * 慣性モーメントはスカラーで近似している(箱や球など、形状の偏りが小さい物体を想定)。
 * 単位は UE に合わせ、長さ [cm]、時間 [s]、角度 [rad]。
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEWMOON_API UZeroGMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZeroGMotionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ======== 運動の状態 ========
	// 初期の速度 [cm/s]
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "ZeroG")
	FVector InitialVelocity = FVector::ZeroVector;

	// 現在の速度 [cm/s]
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZeroG")
	FVector Velocity = FVector::ZeroVector;

	// 現在の角速度 [rad/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG")
	FVector AngularVelocity = FVector::ZeroVector;

	// ======== 物体の性質 ========

	// 質量(プレイヤーとの比で押されたときの飛びやすさが決定)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG", meta = (ClampMin = "0.01"))
	float Mass = 1.f;

	/* 慣性モーメント(スカラー近似)。大きいほど回転しにくい。
	目安：立方体 m*a^2/6、円柱う(横向き) m*(3r^2 + L^2)/12 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG", meta = (ClampMin = "0.01"))
	float MomentOfInertia = 10000.f;

	// 反発係数(0：跳ね返らない ~ 1：完全弾性衝突)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Restitution = 0.7f;

	// 摩擦係数(クーロン摩擦)。接線方向の力積は Friction * 法線方向の力積 が上限
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0"))
	float Friction = 0.4f;

	// ======== 減衰 ========

	// 並進の減衰率[1/s]。約0.7 / DragRate 秒で速さが半分になる
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0"))
	float DragRate = 0.1f;

	// 回転の減衰率 [1/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0"))
	float AngularDrag = 0.5f;

	// ======== 重力・静止判定 ========

	// 重力過疎sく度[cm/s^2]。無重力時は0。SetGravityEnabled で切り替える
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion")
	FVector GravityAcceleration = FVector::ZeroVector;

	/* この速さ未満で接近した衝突は反発させない[cm/s]。
	*  床の上で毎フレーム跳ね返り続けて振動するのを防ぐ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0"))
	float RestingThreshold = 50.f;

	// 床の上でこの速さ未満になったら停止 [cm/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0"))
	float SleepLinearThreshold = 20.f;

	// 床の上でこの角度未満になったら停止 [cm/s]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZeroG | Motion", meta = (ClampMin = "0.0"))
	float SleepAngularThreshold = 0.3f;

	// ======== デバッグ ========

	// 衝突時の速度・エネルギーをログに出力
	UPROPERTY(EditAnywhere, Category = "ZeroG | Debug")
	bool bDrawDebug = false;

	// ======== 外部から呼ぶ処理 ========

	/* ワールド座標の点に力積を加え、速度と角速度を同時に更新。
	*  速度の変化 = J / m。角速度の変化 = (r × J) / I (rは重心から作用点へのベクトル) */ 
	UFUNCTION(BlueprintCallable, Category = "ZeroG | Motion")
	void ApplyImpulseAtPoint(const FVector& Impulse, const FVector& WorldPoint);

	/* 相手(速度と質量の逆数で表す)との接触を解決
	*  壁は OtherVelocity = 0、 OtherInvMass = 0(質量無限大) として扱う。
	*  InNormal は「相手から自分へ」向く法線を渡す。*/
	UFUNCTION(BlueprintCallable, Category = "ZeroG | Motion")
	void ResolveContact(const FVector& ContactPoint, const FVector& InNormal, const FVector& OtherVelocity, float OtherInvMass);

	// 重力の有効・無効を切り替える(重力制御装置の再起動用)
	UFUNCTION(BlueprintCallable, Category = "ZeroG | Motion")
	void SetGravityEnabled(bool bEnabled);

	// 位置・回転・速度を開始時の状態に戻す(リスポーン用)
	UFUNCTION(BlueprintCallable, Category = "ZeroG | Motion")
	void ResetBody();

private:
	// スイープ移動で当たった相手の種類に応じて衝突処理を振り分ける
	void ResolveCollision(const FHitResult& Hit);

	// 浮遊物同士の衝突。両者の質量と慣性を考慮し、作用・反作用の力積を加える
	void ResolveBodyContact(UZeroGMotionComponent* Other, const FVector& ContactPoint, const FVector& InNormal);

	// 開始時のTransform(リスポーン時に戻す)
	FTransform InitialTransform;
};
