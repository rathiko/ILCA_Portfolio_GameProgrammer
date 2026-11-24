// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAppearComponent.h"
#include "EnemyMoveComponent.h"
#include "EnemyAttackComponent.h"
#include "EnemyDistCalculationComponent.h"
#include "Kismet/KismetMathLibrary.h"

UEnemyAppearComponent::UEnemyAppearComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyAppearComponent::BeginPlay()
{
    Super::BeginPlay();

    MoveComponent = GetOwner()->FindComponentByClass<UEnemyMoveComponent>();
    AttackComponent = GetOwner()->FindComponentByClass<UEnemyAttackComponent>();
    DistComponent = GetOwner()->FindComponentByClass<UEnemyDistCalculationComponent>();

    PrevLocation = GetOwner()->GetActorLocation();
}

void UEnemyAppearComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Appear中かつHorizontalのみ回転処理
    if (!bIsAppearing) return;

    switch (AppearType)
    {
    case E_EnemyAppearType::Vertical:
        UpdateVerticalRotation(DeltaTime);
        break;

    case E_EnemyAppearType::Horizontal:
        UpdateHorizontalRotation(DeltaTime);
        break;

    case E_EnemyAppearType::Front:
        // 何もしない
        break;
    }
}

// 登場スタート
void UEnemyAppearComponent::StartAppear()
{
    bIsAppearing = true;
    PrevLocation = GetOwner()->GetActorLocation();

    // BPのTimelineを起動
    OnStartAppear.Broadcast();
}

// 登場演出終了
void UEnemyAppearComponent::OnAppearFinished()
{
    bIsAppearing = false;
    OnStartBattle.Broadcast();

    if (MoveComponent)
    {
        MoveComponent->StopAutoRotation();
        MoveComponent->StartMove();
    }

    if (AttackComponent)
    {
        AttackComponent->StartBattleAim();
        AttackComponent->StartAttack();
    }

    if (DistComponent)
    {
        DistComponent->ChangeState(E_EnemyState::Battle);
    }
}

// 上下から登場する場合の回転
void UEnemyAppearComponent::UpdateVerticalRotation(float DeltaTime)
{
    FVector CurrentLocation = GetOwner()->GetActorLocation();
    FVector Velocity = CurrentLocation - PrevLocation;

    if (!Velocity.IsNearlyZero())
    {
        FRotator TargetRot = UKismetMathLibrary::MakeRotFromX(Velocity);
        TargetRot.Yaw += 180.f;
        TargetRot.Pitch = FMath::Clamp(TargetRot.Pitch, -20.f, 20.f);
        TargetRot.Roll = 0.f;

        FRotator CurrentRot = GetOwner()->GetActorRotation();
        FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 5.f);
        GetOwner()->SetActorRotation(NewRot);
    }

    PrevLocation = CurrentLocation;
}

// 左右から登場する場合の回転
void UEnemyAppearComponent::UpdateHorizontalRotation(float DeltaTime)
{
    FVector CurrentLocation = GetOwner()->GetActorLocation();
    FVector Velocity = CurrentLocation - PrevLocation;

    if (!Velocity.IsNearlyZero())
    {
        // -X軸方向を向かせる
        FRotator TargetRot = FRotator(0.f, 180.f, 0.f);

        // VelocityのY方向で左右を判定
        // 左から右（Y+方向）→ Roll = -10
        // 右から左（Y-方向）→ Roll = +10
        if (Velocity.Y > 0.f)
        {
            TargetRot.Roll = -MaxTiltAngle;
        }
        else
        {
            TargetRot.Roll = MaxTiltAngle;
        }

        FRotator NewRot = FMath::RInterpTo(GetOwner()->GetActorRotation(), TargetRot, DeltaTime, RotationInterpSpeed);

        GetOwner()->SetActorRotation(NewRot);
    }

    PrevLocation = CurrentLocation;
}

// PlayerのRespawn時に呼び出す関数
void UEnemyAppearComponent::ResetAppear()
{
    // 回転を初期状態に戻す
    FRotator ResetRot = FRotator(0.f, 180.f, 0.f);

    // Horizontalの場合は傾きも初期化
    if (AppearType == E_EnemyAppearType::Horizontal)
    {
        if (GetOwner()->GetActorLocation().Y > 0.f)
        {
            ResetRot.Roll = -MaxTiltAngle;
        }
        else
        {
            ResetRot.Roll = MaxTiltAngle;
        }
    }

    GetOwner()->SetActorRotation(ResetRot);

    // フラグリセット
    bIsAppearing = false;
    PrevLocation = GetOwner()->GetActorLocation();
}