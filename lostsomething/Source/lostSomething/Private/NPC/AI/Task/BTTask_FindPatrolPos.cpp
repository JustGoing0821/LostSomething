// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/Task/BTTask_FindPatrolPos.h"
#include "NPC/AI/TestNPCAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get Pawn
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // 컨트롤 중인 Pawn 가져오기
    APawn* ControllingPawn = AIController->GetPawn();
    if (!ControllingPawn)
    {
        return EBTNodeResult::Failed;
    }

    // 내비게이션 시스템 가져오기
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(ControllingPawn->GetWorld());
    if (!NavSystem)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    if (!BlackboardComp->IsVectorValueSet(ATestNPCAIController::Key_HomePos))
    {
        UE_LOG(LogTemp, Warning, TEXT("UBTTask_FindPatrolPos: Key_HomePos is NOT set in Blackboard!"));
    }

    FVector Origin = BlackboardComp->GetValueAsVector(ATestNPCAIController::Key_HomePos);

    // HomePos 값이 너무 크거나 NaN인지 확인
    if (Origin.ContainsNaN())
    {
        UE_LOG(LogTemp, Warning, TEXT("UBTTask_FindPatrolPos: Invalid Home Position Detected! Resetting to Pawn Location"));
        Origin = ControllingPawn->GetActorLocation();
    }


    // 순찰 위치 찾기
    FNavLocation NextPatrol;
    if (NavSystem->GetRandomPointInNavigableRadius(Origin, 600.0f, NextPatrol))
    {
        // 순찰 위치 저장
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(ATestNPCAIController::Key_PatrolPos, NextPatrol.Location);
        UE_LOG(LogTemp, Log, TEXT("UBTTask_FindPatrolPos: Patrol Position Set To = %s"), *NextPatrol.Location.ToString());
        return EBTNodeResult::Succeeded;
    }

    UE_LOG(LogTemp, Warning, TEXT("UBTTask_FindPatrolPos: Failed to find a valid patrol position"));
    return EBTNodeResult::Failed;
}
