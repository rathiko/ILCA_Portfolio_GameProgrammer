// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class E_EnemyMoveType : uint8
{
	Nothing		UMETA(DisplayName = "Nothing"),
	Straight	UMETA(DisplayName = "Straight"),
	Passing		UMETA(DisplayName = "Passing"),
	Random		UMETA(DisplayName = "Random"),
	Cycle		UMETA(DisplayName = "Cycle")
};