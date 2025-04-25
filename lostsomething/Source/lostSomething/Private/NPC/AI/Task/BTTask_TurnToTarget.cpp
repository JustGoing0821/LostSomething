// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/Task/BTTask_TurnToTarget.h"
#include "NPC/AI/TestNPCAIController.h"
#include "NPC/TestNPC.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

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

    ATestNPC* AICharacter = Cast<ATestNPC>(ControllingPawn);
    if (nullptr == AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATestNPCAIController::Key_Target));
    if (nullptr == TargetPawn)
    {
        return EBTNodeResult::Failed;
    }

    float TurnSpeed = 100.0f;
    FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
    LookVector.Z = 0.0f;
    FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
    ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), TurnSpeed));

    return EBTNodeResult::Succeeded;

}