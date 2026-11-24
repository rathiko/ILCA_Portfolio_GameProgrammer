#pragma once

#include "CoreMinimal.h"
#include "DistanceCalculationComponent.h"
#include "E_EnemyState.h"
#include "EnemyAppearComponent.h" 
#include "EnemyDistCalculationComponent.generated.h"

class UEnemyMoveComponent;
class UEnemyAttackComponent;
class UEnemyAppearComponent;

// 退避イベント
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRetreat);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class SPACESHOOTING_API UEnemyDistCalculationComponent : public UDistanceCalculationComponent
{
	GENERATED_BODY()

public:
	UEnemyDistCalculationComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime,ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 現在の状態
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_EnemyState CurrentState = E_EnemyState::Hidden;

	// 距離設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RetreatDistance = 30000.f;

	// 状態変更
	UFUNCTION(BlueprintCallable)
	void ChangeState(E_EnemyState NewState);

	// Retreat開始イベント
	UPROPERTY(BlueprintAssignable)
	FOnStartRetreat OnStartRetreat;

	// Battle開始関数
	UFUNCTION(BlueprintCallable)
	void StartBattle();

private:

	UEnemyMoveComponent* MoveComponent;

	UEnemyAttackComponent* AttackComponent;

	UEnemyAppearComponent* AppearComponent;
};