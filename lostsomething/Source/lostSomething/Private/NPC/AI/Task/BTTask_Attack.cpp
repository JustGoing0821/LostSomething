// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/Task/BTTask_Attack.h"
#include "NPC/AI/TestNPCAIController.h"
#include "NPC/TestNPC.h"
#include "Interface/TestNPCInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AICon = OwnerComp.GetAIOwner();
    ATestNPC* NPC = Cast<ATestNPC>(AICon->GetPawn());

    if (!NPC || NPC->bIsAttacking) return EBTNodeResult::Failed;

    // 콤보 0 시작
    NPC->AttackByAI();

    // Blackboard에 공격 중임을 표시 (필수 아님, 조건에 따라)
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bIsAttacking"), true);

    return EBTNodeResult::Succeeded;
}
