// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAttackComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UEnemyAttackComponent::UEnemyAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void UEnemyAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Homing中のみTickでプレイヤー方向に向き続ける
	if (bIsHomingActive)
	{
		UpdateTargetRotation();

		GetOwner()->SetActorRotation(FMath::RInterpTo(GetOwner()->GetActorRotation(), TargetRotation, DeltaTime, 2.f));
	}
}

// Battle開始時にAimTypeに応じた照準処理を開始する
void UEnemyAttackComponent::StartBattleAim()
{
	// Noneは照準処理なし
	if (AimType == E_EnemyAimType::None)
	{
		return;
	}

	// Turret：登場演出後にプレイヤーの方向へ回転
	if (AimType == E_EnemyAimType::Turret)
	{
		UpdateTargetRotation();

		// BPでプレイヤー方向を向くTimeline開始
		OnStartTurret.Broadcast();
	}

	// Homing：常時プレイヤーの方向を向き続ける
	if (AimType == E_EnemyAimType::Homing)
	{
		bIsHomingActive = true;

		UpdateTargetRotation();
	}
}

// 照準を停止する
void UEnemyAttackComponent::StopAim()
{
	bIsHomingActive = false;
}

void UEnemyAttackComponent::StartAttack()
{
	bCanAttack = true;
}

void UEnemyAttackComponent::StopAttack()
{
	bCanAttack = false;
}

// プレイヤーへのLookAt回転を計算してTragetRotationに保存
void UEnemyAttackComponent::UpdateTargetRotation()
{
	if (!CachedPlayerPawn)
	{
		return;
	}

	FVector MyLocation = GetOwner()->GetActorLocation();

	FVector PlayerLocation = CachedPlayerPawn->GetActorLocation();

	TargetRotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, PlayerLocation);
}