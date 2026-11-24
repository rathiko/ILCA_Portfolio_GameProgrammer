// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsyncLevelLoader.generated.h"

UCLASS()
class SPACESHOOTING_API AAsyncLevelLoader : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAsyncLevelLoader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void OnLevelLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);

	UPROPERTY(EditAnywhere)
	FString NextLevelPath;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartFadeOut();

	UFUNCTION(BlueprintCallable)
	void OpenNextLevel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextLevel;
};