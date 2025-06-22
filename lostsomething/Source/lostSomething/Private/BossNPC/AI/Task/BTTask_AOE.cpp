// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/Task/BTTask_AOE.h"
#include "BossNPC/AI/BossNPCAIController.h"
#include "BossNPC/BossNPC.h"

UBTTask_AOE::UBTTask_AOE()
{

}

EBTNodeResult::Type UBTTask_AOE::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!ControlledPawn.IsValid())
    {
        return EBTNodeResult::Failed;
    }

    ABossNPC* BossPawn = Cast<ABossNPC>(ControlledPawn);
    if (!BossPawn)
    {
        return EBTNodeResult::Failed;
    }

    BossPawn->AOEMontagePlay();

    return EBTNodeResult::Succeeded;
}
