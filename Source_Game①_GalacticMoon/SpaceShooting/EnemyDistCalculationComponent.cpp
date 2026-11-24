// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyDistCalculationComponent.h"
#include "EnemyMoveComponent.h"
#include "EnemyAttackComponent.h"

UEnemyDistCalculationComponent::UEnemyDistCalculationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyDistCalculationComponent::BeginPlay()
{
	Super::BeginPlay();

	MoveComponent = GetOwner() -> FindComponentByClass<UEnemyMoveComponent>();

	AttackComponent = GetOwner()->FindComponentByClass<UEnemyAttackComponent>();

	AppearComponent = GetOwner()->FindComponentByClass<UEnemyAppearComponent>();
}

void UEnemyDistCalculationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Hidden Å® Appear
	if (bIsNearPlayer && CurrentState == E_EnemyState::Hidden)
	{
		ChangeState(E_EnemyState::Appear);

		if (AppearComponent)
		{
			AppearComponent->ResetAppear();
			AppearComponent->StartAppear();
		}
	}

	// Battle Å® Retreat
	if (DistanceX < RetreatDistance && CurrentState == E_EnemyState::Battle)
	{
		ChangeState(E_EnemyState::Retreat);

		OnStartRetreat.Broadcast();

		if (MoveComponent)
		{
			MoveComponent->StopMove();
		}

		if (AttackComponent)
		{
			AttackComponent->StopAttack();
		}
	}

	// îÒï\é¶îªíË
	if (!bIsNearPlayer && CurrentState != E_EnemyState::Hidden)
	{
		ChangeState(E_EnemyState::Hidden);
	}
}

void UEnemyDistCalculationComponent::ChangeState(E_EnemyState NewState)
{
	CurrentState = NewState;
}

// ìoèÍââèoå„Ç…åƒÇ—èoÇ∑ä÷êî
void UEnemyDistCalculationComponent::StartBattle()
{
	ChangeState(E_EnemyState::Battle);
	
	if (MoveComponent)
	{
		MoveComponent->StartMove();
	}

	if (AttackComponent)
	{
		AttackComponent->StartAttack();
	}

}
