#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "E_EnemyAimType.h"
#include "EnemyAttackComponent.generated.h"

// Turret用のTimeline開始イベント
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartTurret);

// 敵の照準・攻撃処理を管理するComponent
// AimTypeのEnumで照準パターンを切り替える
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class SPACESHOOTING_API UEnemyAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UEnemyAttackComponent();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	// 照準パターンの選択
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	E_EnemyAimType AimType = E_EnemyAimType::None;

	// プレイヤーのPawnをキャッシュ (BeginPlayで取得)
	UPROPERTY()
	APawn* CachedPlayerPawn;

	// Homingが有効かどうか (TickでプレイヤーへのLookAtを更新)
	UPROPERTY(BlueprintReadOnly)
	bool bIsHomingActive = false;

	// 自動回転 (TurretとHomingで使用)
	UPROPERTY(BlueprintReadOnly)
	FRotator TargetRotation;

	// Battle開始時に呼ばれる (AimTypeに応じた照準処理を開始)
	UFUNCTION(BlueprintCallable)
	void StartBattleAim();

	// 照準停止 (Retreat時・やられたときに呼ばれる)
	UFUNCTION(BlueprintCallable)
	void StopAim();

	// プレイヤー方向にTargetRotationを更新
	UFUNCTION(BlueprintCallable)
	void UpdateTargetRotation();

	// 攻撃フラグをONにする (BPの攻撃処理を連動)
	UFUNCTION(BlueprintCallable)
	void StartAttack();

	// 攻撃フラグをOFFにする
	UFUNCTION(BlueprintCallable)
	void StopAttack();

	// TurretのTimeline開始をBPに通知
	UPROPERTY(BlueprintAssignable)
	FOnStartTurret OnStartTurret;

private:
	// 攻撃処理のON/OFFフラグ(BPの攻撃Tickと連動)
	bool bCanAttack = false;
};