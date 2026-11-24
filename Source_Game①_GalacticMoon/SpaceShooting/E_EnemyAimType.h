// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class E_EnemyAimType : uint8
{
	None		UMETA(DisplayName = "None"),
	Turret		UMETA(DisplayName = "Turret"),
	Homing		UMETA(DisplayName = "Homing")
};