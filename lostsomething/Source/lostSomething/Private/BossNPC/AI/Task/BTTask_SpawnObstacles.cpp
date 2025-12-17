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

    if (TimeSinceLastSpawn >= 1.0f)
    {
        if (CurrentSpawnCount < 8)
        {
            ABossNPC* BossPawn = Cast<ABossNPC>(ControlledPawn.Get());
            BossPawn->ObsMontagePlay();
            TimeSinceLastSpawn = 0.0f;
            CurrentSpawnCount++;
        }
        else
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}