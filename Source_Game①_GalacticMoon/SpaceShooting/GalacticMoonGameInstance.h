// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GalacticMoonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHOOTING_API UGalacticMoonGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

    UPROPERTY(BlueprintReadWrite)
    FName NextLevelName;
};
