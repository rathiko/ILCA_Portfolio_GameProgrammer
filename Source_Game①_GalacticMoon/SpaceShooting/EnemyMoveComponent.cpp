// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyMoveComponent.h"
#include "Kismet/KismetMathLibrary.h"

UEnemyMoveComponent::UEnemyMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();

	// ‰ŠúˆÊ’u‚ð•Û‘¶(RandomECycle‚ÌŠî€“_‚Æ‚µ‚ÄŽg—p)
	if (OwnerActor)
	{
		StartLocation = OwnerActor->GetActorLocation();
	}
}

// ˆÚ“®ˆ—‚ðŠJŽn‚·‚é
void UEnemyMoveComponent::StartMove()
{
	bCanMove = true;
}

// ˆÚ“®ˆ—‚ð’âŽ~‚·‚é
void UEnemyMoveComponent::StopMove()
{
	bCanMove = false;
}

// Ž©“®‰ñ“]‚ð’âŽ~‚·‚é (BattleˆÚsŽž‚ÉAppearComponent‚©‚çŒÄ‚Î‚ê‚é)
void UEnemyMoveComponent::StopAutoRotation()
{
	bAutoRotation = false;
}

void UEnemyMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bCanMove || !OwnerActor)
	{
		return;
	}

	// Œo‰ßŽžŠÔ‚ð‰ÁŽZ(SinECosŒvŽZ‚ÉŽg—p)
	Time += DeltaTime;

	FVector Location = OwnerActor->GetActorLocation();

	// MoveType‚É‰ž‚¶‚½ˆÚ“®ˆ—
	switch (MoveType)
	{
	case E_EnemyMoveType::Nothing:
	{
		// ˆÚ“®‚È‚µ
		break;
	}

	case E_EnemyMoveType::Straight:
	{
		// Žw’è•ûŒü‚É’¼i
		Location += MoveDirection * MoveSpeed * DeltaTime;

		break;
	}

	case E_EnemyMoveType::Passing:
	{
		// Ž©g‚Ì‘O•ûŒü‚É’¼i(’Ê‚è‰ß‚¬‚é)
		Location += OwnerActor->GetActorForwardVector() * MoveSpeed * DeltaTime;

		break;
	}

	case E_EnemyMoveType::Random:
	{
		// X•ûŒü‚Éi‚Ý‚È‚ª‚çZŽ²‚ðSin”g‚Åã‰º‚É—h‚ê‚é
		Location += MoveDirection * MoveSpeed * DeltaTime;

		Location.Z = StartLocation.Z + FMath::Sin(Time * MoveFrequency) * MoveRange;

		break;
	}

	case E_EnemyMoveType::Cycle:
	{
		// X•ûŒü‚Éi‚Ý‚È‚ª‚çYEZŽ²‚Å‰~‹O“¹‚ð•`‚­
		Location += MoveDirection * MoveSpeed * DeltaTime;

		Location.Y = StartLocation.Y + FMath::Cos(Time * CycleSpeed) * CycleRadius;

		Location.Z = StartLocation.Z + FMath::Sin(Time * CycleSpeed) * CycleRadius;

		break;
	}
	}

	OwnerActor->SetActorLocation(Location);
}