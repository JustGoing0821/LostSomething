// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/BMSpawner.h"
#include <BossNPC/BossNPC.h>

// Sets default values
ABMSpawner::ABMSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	static ConstructorHelpers::FClassFinder<AActor> MonsterBP(TEXT("/Game/BossNPC/BluePrints/BP_BossNPC.BP_BossNPC_C"));
	if (MonsterBP.Succeeded())
	{
		MonsterClass = MonsterBP.Class;
	}

}

// Called when the game starts or when spawned
void ABMSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}


	for (const FVector& Offset : SpawnOffsets)
	{
		//FVector SpawnLocation = GetActorLocation() + Offset;
		FVector SpawnLocation = GetActorLocation();
		GetWorld()->SpawnActor<ABossNPC>(MonsterClass, SpawnLocation, GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("SpawnActor"))
	}
	
}

// Called every frame
void ABMSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

