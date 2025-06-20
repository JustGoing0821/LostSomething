// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/Task/BTTask_Platfrom.h"
#include "BossNPC/BossNPC.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossNPC/AI/BossNPCAIController.h"

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

    if (AIController->GetBlackboardComponent()->GetValueAsBool(FName("bPhase3")))
    {
        return EBTNodeResult::Failed;
    }

    // 장판 생성
    BossPawn->EnterPhase3();

    // 재실행 방지용 플래그 설정
    AIController->GetBlackboardComponent()->SetValueAsBool(FName("bPhase3"), true);

    return EBTNodeResult::Succeeded;

}