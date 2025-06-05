// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/BossNPC.h"
#include "BossNPC/AI/BossNPCAIController.h"
#include "BossNPC/Obstacle/BossObstacle.h"
#include "BossNPC/Platform/PlatformGenerator.h"
#include "lostSomething.h"

// Sets default values
ABossNPC::ABossNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// AIController 클래스를 설정
	AIControllerClass = ABossNPCAIController::StaticClass();

	// AIController가 자동으로 소환되도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 예시: SceneComponent 생성
	for (int i = 0; i < 3; ++i)
	{
		FString Name = FString::Printf(TEXT("SpawnPoint_%d"), i);
		USceneComponent* SpawnPoint = CreateDefaultSubobject<USceneComponent>(*Name);
		SpawnPoint->SetupAttachment(RootComponent);
		float YOffset = (i - 1) * 220.f;
		SpawnPoint->SetRelativeLocation(FVector(110.f, YOffset, 20.f));
		ObstacleSpawnPoints.Add(SpawnPoint);
	}

	CurrentHP = MaxHP;
}

// Called when the game starts or when spawned
void ABossNPC::BeginPlay()
{
	Super::BeginPlay();

	EnterPhase3();
}

// Called every frame
void ABossNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABossNPC::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	SetHP(GetHP()-DamageAmount);
	LS_LOG(LogLS, Log, TEXT("CurrentHP : %f"), CurrentHP)

	ABossNPCAIController* PC = Cast<ABossNPCAIController>(GetController());
	if (PC)
	{
		PC->ChangedHP();
	}

	return 0.0f;
}

void ABossNPC::SpawnObstacles()
{
	ServerSpawnObstacles();
}

void ABossNPC::ServerSpawnObstacles_Implementation()
{
	MultiSpawnObstacles();
}

void ABossNPC::MultiSpawnObstacles_Implementation()
{
	TArray<int32> Indexes = { 0, 1, 2 };
	// 랜덤 셔플
	for (int32 i = 0; i < Indexes.Num(); ++i)
	{
		int32 RandIdx = FMath::RandRange(i, Indexes.Num() - 1);
		Indexes.Swap(i, RandIdx);
	}

	// 1 또는 2개 선택
	int32 NumToSpawn = FMath::RandBool() ? 1 : 2;
	for (int32 i = 0; i < NumToSpawn; ++i)
	{
		USceneComponent* SpawnPoint = ObstacleSpawnPoints[Indexes[i]];
		FVector SpawnLocation = SpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = GetActorRotation(); // 보스의 정면 방향

		FActorSpawnParameters Params;
		GetWorld()->SpawnActor<ABossObstacle>(ABossObstacle::StaticClass(), SpawnLocation, SpawnRotation, Params);
	}
}

void ABossNPC::EnterPhase3()
{
	if (HasAuthority() && PlatformGeneratorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering Phase 3 - Spawning Platform Generator"));

		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PlatformGenerator = GetWorld()->SpawnActor<APlatformGenerator>(PlatformGeneratorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

		if (PlatformGenerator)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlatformGenerator spawned successfully"));
			SpawnPlatform();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn PlatformGenerator"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterPhase3 skipped - No Authority or GeneratorClass is null"));
	}
}

void ABossNPC::SpawnPlatform()
{
	if (HasAuthority()) // 서버인지 확인
	{
		if (PlatformGenerator)
		{
			PlatformGenerator->GenerateMaze(); // 핵심 로직 호출
		}
	}
	else
	{
		ServerSpawnPlatform(); // 클라일 경우 서버 RPC 요청

	}
}

void ABossNPC::ServerSpawnPlatform_Implementation()
{
	SpawnPlatform();
}

void ABossNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ABossNPC, bIsAttacking);
}