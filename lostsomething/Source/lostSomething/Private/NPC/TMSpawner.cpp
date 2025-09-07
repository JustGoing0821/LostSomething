// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/TMSpawner.h"
#include <NPC/TestNPC.h>

// Sets default values
ATMSpawner::ATMSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	static ConstructorHelpers::FClassFinder<AActor> MonsterBP(TEXT("/Game/NPC/Blueprints/BP_TestNPC.BP_TestNPC_C"));
	if (MonsterBP.Succeeded())
	{
		MonsterClass = MonsterBP.Class;
	}

}

// Called when the game starts or when spawned
void ATMSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}


	if (SpawnOffsets.Num() == 0) // 만약 에디터에서 입력 안 되어 있다면 기본값 채우기
	{
		SpawnOffsets.Add(FVector(0.f, 0.f, 0.f));
	}

	FTimerHandle NPCSpawnTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		NPCSpawnTimerHandle,
		this,
		&ATMSpawner::SpawnNPC,
		7.0f,
		false // 반복 여부 (false면 한 번만 실행)
	);
	
}

void ATMSpawner::SpawnNPC()
{
	for (const FVector& Offset : SpawnOffsets)
	{
		FVector SpawnLocation = GetActorLocation() + Offset;
		GetWorld()->SpawnActor<ATestNPC>(MonsterClass, SpawnLocation, GetActorRotation());

	}
}

// Called every frame
void ATMSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

