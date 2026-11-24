// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncLevelLoader.h"
#include "UObject/UObjectGlobals.h"
#include "GalacticMoonGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAsyncLevelLoader::AAsyncLevelLoader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAsyncLevelLoader::BeginPlay()
{
	Super::BeginPlay();

	UGalacticMoonGameInstance* GameInstance =
		Cast<UGalacticMoonGameInstance>(GetGameInstance());

	NextLevel = GameInstance->NextLevelName;

	NextLevelPath = TEXT("/Game/SpaceShooting/Level/") + NextLevel.ToString();

	FLoadPackageAsyncDelegate Delegate;

	Delegate.BindUObject(this, &AAsyncLevelLoader::OnLevelLoaded);

	LoadPackageAsync(NextLevelPath, Delegate, 0, PKG_ContainsMap, INDEX_NONE);
}

void AAsyncLevelLoader::OnLevelLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
{
	if (Result == EAsyncLoadingResult::Succeeded)
	{
		UE_LOG(LogTemp, Log, TEXT("NextLevel loading completed!"));

		StartFadeOut();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NextLevel loading failed")
		);
	}
}

void AAsyncLevelLoader::OpenNextLevel()
{
	UGameplayStatics::OpenLevel(this, NextLevel);
}