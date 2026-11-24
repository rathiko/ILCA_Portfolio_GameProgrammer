// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "EnemyRandomMovement.h"


// Sets default values for this component's properties
UEnemyRandomMovement::UEnemyRandomMovement()
{

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UEnemyRandomMovement::BeginPlay()
{
	Super::BeginPlay();

	//Å‰‚ÌŒü‚«
	InitialRotation = GetOwner()->GetActorRotation();

	// Å‰‚Ìƒ‰ƒ“ƒ_ƒ€ˆÚ“®
	RandomDirection();

}

// Called every frame
void UEnemyRandomMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AActor* Owner = GetOwner();

	if (!Owner || !PlayerPawn) return;



	//“G‚ÆƒvƒŒƒCƒ„[ŠÔ‚Ì‹——£‚ðŽæ“¾
	float Distance = FVector::Dist(Owner->GetActorLocation(), PlayerPawn->GetActorLocation());

	if (Distance < MoveStartDistance)
	{
		ElapsedTime += DeltaTime;
		if (ElapsedTime >= ChangeInterval)
		{
			ElapsedTime = 0.f;
			RandomDirection();
		}
		if (!Owner) return;


		//ˆÚ“®
		FVector MoveX = FVector(1.f, 0.f, 0.f) * ForwardSpeed * DeltaTime;
		FVector MoveYZ = FVector(0.f, MoveDirection.Y, MoveDirection.Z) * SideSpeed * DeltaTime;

		FVector CurrentLocation = Owner->GetActorLocation();
		FVector Delta = MoveX + MoveYZ;
		FVector NewLocation = CurrentLocation + Delta;

		// ˆÚ“®”ÍˆÍ§ŒÀ
		NewLocation.Y = FMath::Clamp(NewLocation.Y, MinY, MaxY);
		NewLocation.Z = FMath::Clamp(NewLocation.Z, MinZ, MaxZ);

		// sweep=true‚Åˆê“x‚¾‚¯“K—p
		Owner->SetActorLocation(NewLocation, true);

		//‰ñ“]‚ÆƒvƒŒƒCƒ„[‚Ö‚Ì•ûŒüŒvŽZ
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		FVector Toplayer = (PlayerLocation - CurrentLocation).GetSafeNormal();
		FRotator CurrentRotation = Owner->GetActorRotation();

		//“G‚ÌUŒ‚ƒ^ƒCƒv•ÏX
		switch (AttackType)
		{
			//‚Ü‚Á‚·‚®‚ÉUŒ‚
		case EEnemyAttackType::Straight:
			break;

		case EEnemyAttackType::Bomb:
			break;

			//”ÍˆÍ“à‚Å‘_‚¢‚¤‚¿

		case EEnemyAttackType::Turret:
		{
			//“àÏ‚ðŒvŽZ
			float DotProduct = FVector::DotProduct(InitialRotation.Vector(), Toplayer);
			float AngleScope = FMath::Cos(FMath::DegreesToRadians(MaxTrackingAngle));

			if (DotProduct > AngleScope)
			{
				//‘_‚¢Œ‚‚¿
				FVector DirectionToPlayer = PlayerLocation - CurrentLocation;
				FRotator TargetRotation = DirectionToPlayer.Rotation();

				//ŠŠ‚ç‚©‚É‰ñ“]
				FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, HomingRotationSpeed);

				Owner->SetActorRotation(NewRotation);
			}
			else
			{
				FRotator BackRotation = FMath::RInterpTo(Owner->GetActorRotation(), InitialRotation, DeltaTime, HomingRotationSpeed);

				Owner->SetActorRotation(BackRotation);
			}
			break;
		}

		//’Ê‚è‰ß‚¬‚Ä‚à‘_‚¢Œ‚‚¿
		case EEnemyAttackType::Homing:
			FRotator TargetRotation = Toplayer.Rotation();
			Owner->SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, HomingRotationSpeed));
			break;
		}
	}
}

//ƒ‰ƒ“ƒ_ƒ€ˆÚ“®
void UEnemyRandomMovement::RandomDirection()
{
	float RandomY = FMath::RandRange(-1.f, 1.f);
	float RandomZ = FMath::RandRange(-1.f, 1.f);

	MoveDirection = FVector(0.f, RandomY, RandomZ).GetSafeNormal();
}
