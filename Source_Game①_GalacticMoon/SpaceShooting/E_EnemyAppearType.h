// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class E_EnemyAppearType : uint8
{
	Horizontal	 UMETA(DisplayName = "Horizontal"),
	Vertical	 UMETA(DisplayName = "Vertical"),
	Front		 UMETA(DisplayName = "Front")
};