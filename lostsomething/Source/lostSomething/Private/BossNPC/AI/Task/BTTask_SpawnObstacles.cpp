// Fill out your copyright notice in the Description page of Project Settings.

#include "BossNPC/AI/Task/BTTask_SpawnObstacles.h"
#include "BossNPC/AI/BossNPCAIController.h"
#include "BossNPC/BossNPC.h"
#include "BossNPC/Obstacle/BossObstacle.h"


UBTTask_SpawnObstacles::UBTTask_SpawnObstacles()
{
    bNotifyTick = true;
    CurrentSpawnCount = 0;
    TimeSinceLastSpawn = 0.0f;
}

EBTNodeResult::Type UBTTask_SpawnObstacles::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!ControlledPawn.IsValid())
    {
        return EBTNodeResult::Failed;
    }

    CurrentSpawnCount = 0;
    TimeSinceLastSpawn = 0.0f;
    return EBTNodeResult::InProgress;
}

void UBTTask_SpawnObstacles::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    TimeSinceLastSpawn += DeltaSeconds;

    if (TimeSinceLastSpawn >= 1.5f)
    {
        if (CurrentSpawnCount < 3)
        {
            SpawnObstacles(ControlledPawn.Get());
            TimeSinceLastSpawn = 0.0f;
            CurrentSpawnCount++;
        }
        else
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}

void UBTTask_SpawnObstacles::SpawnObstacles(AActor* BossActor)
{
    ABossNPC* Boss = Cast<ABossNPC>(BossActor);
    if (!Boss || Boss->ObstacleSpawnPoints.Num() < 3) return;

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
        USceneComponent* SpawnPoint = Boss->ObstacleSpawnPoints[Indexes[i]];
        FVector SpawnLocation = SpawnPoint->GetComponentLocation();
        FRotator SpawnRotation = Boss->GetActorRotation(); // 보스의 정면 방향

        FActorSpawnParameters Params;
        Boss->GetWorld()->SpawnActor<ABossObstacle>(ABossObstacle::StaticClass(), SpawnLocation, SpawnRotation, Params);
    }
}