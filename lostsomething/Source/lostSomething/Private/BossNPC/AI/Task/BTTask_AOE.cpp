// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/Task/BTTask_AOE.h"
#include "BossNPC/AI/BossNPCAIController.h"
#include "BossNPC/BossNPC.h"

UBTTask_AOE::UBTTask_AOE()
{
	bNotifyTick = true;
	CurrentSpawnCount = 0;
	TimeSinceLastSpawn = 0.0f;
}

EBTNodeResult::Type UBTTask_AOE::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

void UBTTask_AOE::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    TimeSinceLastSpawn += DeltaSeconds;

    if (TimeSinceLastSpawn >= 1.5f)
    {
        if (CurrentSpawnCount < 3)
        {
            ABossNPC* BossPawn = Cast<ABossNPC>(ControlledPawn.Get());
            BossPawn->EnterPhase1();
            TimeSinceLastSpawn = 0.0f;
            CurrentSpawnCount++;
        }
        else
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
