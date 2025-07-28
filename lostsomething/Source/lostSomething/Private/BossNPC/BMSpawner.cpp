// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/BMSpawner.h"
#include <BossNPC/BossNPC.h>

// Sets default values
ABMSpawner::ABMSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	static ConstructorHelpers::FClassFinder<ABossNPC> MonsterBP(TEXT("/Game/BossNPC/BluePrints/BP_BossNPC.BP_BossNPC_C"));
	if (MonsterBP.Succeeded())
	{
		MonsterClass = MonsterBP.Class;
	}

}

// Called when the game starts or when spawned
void ABMSpawner::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BM : Spawner BeginPlay called."));

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("BM : No authority (client). Skipping spawn."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("BM : Has authority (server). Attempting to spawn boss."));

	if (!MonsterClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BM : MonsterClass is nullptr. Cannot spawn boss."));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABossNPC* SpawnedBoss = GetWorld()->SpawnActor<ABossNPC>(
		MonsterClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams
	);

	if (!SpawnedBoss)
	{
		UE_LOG(LogTemp, Error, TEXT("BM : Failed to spawn BossNPC."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BM : BossNPC spawned successfully at: %s"), *SpawnedBoss->GetActorLocation().ToString());
	}
}

// Called every frame
void ABMSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

