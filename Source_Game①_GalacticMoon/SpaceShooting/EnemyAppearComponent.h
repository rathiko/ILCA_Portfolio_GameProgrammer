// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "E_EnemyAppearType.h"
#include "EnemyAppearComponent.generated.h"

class UEnemyMoveComponent;
class UEnemyAttackComponent;
class UEnemyDistCalculationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartAppear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartBattle);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class SPACESHOOTING_API UEnemyAppearComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyAppearComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // 登場タイプ
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    E_EnemyAppearType AppearType = E_EnemyAppearType::Front;

    // 傾き最大角度（Horizontal用）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxTiltAngle = 20.f;

    // 補間速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RotationInterpSpeed = 5.f;

    // Appear中フラグ
    UPROPERTY(BlueprintReadOnly)
    bool bIsAppearing = false;

    // BPのTimeline開始を通知
    UPROPERTY(BlueprintAssignable)
    FOnStartAppear OnStartAppear;

    // Timeline終了時に呼び出し
    UPROPERTY(BlueprintAssignable)
    FOnStartBattle OnStartBattle;

    // DistCalcから呼ばれる
    UFUNCTION(BlueprintCallable)
    void StartAppear();

    // BPのTimeline終了後に呼ば出す関数
    UFUNCTION(BlueprintCallable)
    void OnAppearFinished();

    // Respawn時に呼び出す関数
    UFUNCTION(BlueprintCallable)
    void ResetAppear();

private:
    UEnemyMoveComponent* MoveComponent;
    UEnemyAttackComponent* AttackComponent;
    UEnemyDistCalculationComponent* DistComponent;

    // Horizontal用：移動方向ベースの回転処理
    void UpdateHorizontalRotation(float DeltaTime);

    // Vertical用：移動方向ベースの回転処理
    void UpdateVerticalRotation(float DeltaTime);

    FVector PrevLocation;
};