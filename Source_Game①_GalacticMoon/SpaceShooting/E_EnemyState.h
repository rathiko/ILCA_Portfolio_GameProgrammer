// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
		enum class E_EnemyState : uint8
	{
		Hidden	UMETA(DisplayName = "Hidden"),
		Appear	UMETA(DisplayName = "Appear"),
		Battle	UMETA(DisplayName = "Battle"),
		Retreat UMETA(DisplayName = "Retreat")
	};