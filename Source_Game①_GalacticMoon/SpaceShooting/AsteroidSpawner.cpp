// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidSpawner.h"
#include "Kismet/GameplayStatics.h"

AAsteroidSpawner::AAsteroidSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	// Instance Static Mesh(ISM)コンポーネントを作成
	AsteroidISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Asteroid"));

	RootComponent = AsteroidISM;

	// 当たり判定を有効に
	AsteroidISM->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AsteroidISM->SetCollisionObjectType(ECC_WorldStatic);

	// すべてのオブジェクトを「ブロック」する設定
	AsteroidISM->SetCollisionResponseToAllChannels(ECR_Block);

	// ISMインスタンスごとのコリジョンを有効にする
	AsteroidISM->SetGenerateOverlapEvents(true);
}

void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();

	CachedPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	RandomStream.Initialize(RandomSeed);

	if (!AsteroidMesh) return;
	AsteroidISM->SetStaticMesh(AsteroidMesh);

	// 岩生成（最初はすべて非表示用の遠方に配置）
	for (int32 i = 0; i < InstanceCount; ++i)
	{
		FVector RandomLocation(
			RandomStream.FRandRange(-SpawnRange.X, SpawnRange.X),
			RandomStream.FRandRange(-SpawnRange.Y, SpawnRange.Y),
			RandomStream.FRandRange(-SpawnRange.Z, SpawnRange.Z));

		FRotator RandomRotation(
			RandomStream.FRandRange(0.f, 360.f),
			RandomStream.FRandRange(0.f, 360.f),
			RandomStream.FRandRange(0.f, 360.f));

		float RandomScale = RandomStream.FRandRange(MinScale, MaxScale);

		FTransform InitialTransform(RandomRotation, RandomLocation, FVector(RandomScale));

		// 本来の初期位置を保存
		InitialTransforms.Add(InitialTransform);

		// 最初は画面外の遠方に避難させて生成
		FTransform HiddenTransform = InitialTransform;
		HiddenTransform.SetLocation(HiddenLocation);
		AsteroidISM->AddInstance(HiddenTransform);

		// 表示状態フラグをすべて false で初期化
		bIsVisibleArray.Add(false);

		// 回転速度
		FRotator RotationSpeed(
			RandomStream.FRandRange(-10.f, 10.f),
			RandomStream.FRandRange(-20.f, 20.f),
			RandomStream.FRandRange(-20.f, 20.f));
			RotationSpeeds.Add(RotationSpeed);
	}

	// 初回の配置
	ResetAsteroids();
}

void AAsteroidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CachedPlayerPawn) return;

	FVector PlayerLocation = CachedPlayerPawn->GetActorLocation();
	int32 Count = AsteroidISM->GetInstanceCount();

	// 距離チェック（インターバル秒ごとに表示判定を行う）
	DistanceCheckAccumulator += DeltaTime;
	if (DistanceCheckAccumulator >= DistanceCheckInterval)
	{
		DistanceCheckAccumulator = 0.f;
		UpdateAsteroidDistanceVisibility(PlayerLocation);
	}

	// 移動・回転（表示されている岩だけが動く）
	for (int32 i = 0; i < Count; ++i)
	{
		// 非表示（まだ出現前、またはBP側で消された岩）なら計算をスキップ
		if (!bIsVisibleArray[i])
		{
			continue;
		}

		FTransform InstanceTransform;
		if (!AsteroidISM->GetInstanceTransform(i, InstanceTransform, false))
		{
			continue;
		}

		FVector CurrentLocation = InstanceTransform.GetLocation();

		// 横スクロール移動
		CurrentLocation.Y -= ScrollSpeed * DeltaTime;

		// 画面の端に行ったらループ
		if (CurrentLocation.Y < -SpawnRange.Y)
		{
			CurrentLocation.Y = SpawnRange.Y;
			FVector InitialLocation = InitialTransforms[i].GetLocation();
			CurrentLocation.X = InitialLocation.X;
			CurrentLocation.Z = InitialLocation.Z;
		}
		InstanceTransform.SetLocation(CurrentLocation);

		// 自転（回転）処理
		FRotator CurrentRotation = InstanceTransform.Rotator();
		CurrentRotation += RotationSpeeds[i] * DeltaTime;
		InstanceTransform.SetRotation(CurrentRotation.Quaternion());

		AsteroidISM->UpdateInstanceTransform(i, InstanceTransform, false, false, true);
	}

	if (bNeedRenderUpdate)
	{
		AsteroidISM->MarkRenderStateDirty();
		bNeedRenderUpdate = false;
	}
}

// 復活・非表示の双方を安全に行えるようにロジックを修正
void AAsteroidSpawner::SetAsteroidVisible(int32 Index, bool bVisible)
{
	FTransform Transform;
	if (!AsteroidISM->GetInstanceTransform(Index, Transform, false))
	{
		return;
	}

	if (bVisible)
	{
		// 表示フラグがtrueの時は、本来の初期位置（InitialTransforms）に戻す
		Transform = InitialTransforms[Index];
	}
	else
	{
		// 非表示（BP側から呼ばれた時など）は遠くへ飛ばす
		Transform.SetLocation(HiddenLocation);
	}

	AsteroidISM->UpdateInstanceTransform(Index, Transform, false, false, true);
	bIsVisibleArray[Index] = bVisible;
}

void AAsteroidSpawner::ResetAsteroids()
{
	CachedPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!CachedPlayerPawn) return;

	int32 Count = AsteroidISM->GetInstanceCount();

	// 全岩を明示的に非表示位置へ移動してからフラグリセット
	for (int32 i = 0; i < Count; ++i)
	{
		SetAsteroidVisible(i, false);
	}

	bNeedRenderUpdate = true;

	FVector PlayerLocation = CachedPlayerPawn->GetActorLocation();
	UpdateAsteroidDistanceVisibility(PlayerLocation);
}

void AAsteroidSpawner::UpdateAsteroidDistanceVisibility(const FVector& PlayerLocation)
{
	int32 Count = AsteroidISM->GetInstanceCount();
	float SpawnerWorldX = GetActorLocation().X;

	for (int32 i = 0; i < Count; ++i)
	{
		FVector InitialLocation = InitialTransforms[i].GetLocation();
		float AsteroidWorldX = InitialLocation.X + SpawnerWorldX;

		// SpawnerのワールドX座標を加算してプレイヤーとの相対距離を計算
		float DistanceX = AsteroidWorldX - PlayerLocation.X;

		bool bShouldVisible = (DistanceX < VisibleDistance);

		// 現在の状態と目標状態が異なる場合だけ更新
		if (bShouldVisible != bIsVisibleArray[i])
		{
			SetAsteroidVisible(i, bShouldVisible);
			bNeedRenderUpdate = true;
		}
	}
}