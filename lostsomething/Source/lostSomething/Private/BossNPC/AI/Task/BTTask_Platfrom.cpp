// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/Task/BTTask_Platfrom.h"
#include "BossNPC/AI/BossNPCAIController.h"
#include "BossNPC/BossNPC.h"

UBTTask_Platfrom::UBTTask_Platfrom()
{
}

EBTNodeResult::Type UBTTask_Platfrom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABossNPCAIController* AIController = Cast<ABossNPCAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    ABossNPC* BossPawn = Cast<ABossNPC>(AIController->GetPawn());
    if (!BossPawn)
    {
        return EBTNodeResult::Failed;
    }
    BossPawn->EnterPhase3();

    return EBTNodeResult::Succeeded;
}