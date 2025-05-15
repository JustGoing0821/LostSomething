// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/Service/BTService_DetectChaseDist.h"
#include "NPC/AI/TestNPCAIController.h"
#include "NPC/TestNPC.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_DetectChaseDist::UBTService_DetectChaseDist()
{
	NodeName = TEXT("DetectChaseDist");
	Interval = 1.0f;
}

void UBTService_DetectChaseDist::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AICon = OwnerComp.GetAIOwner();
    ATestNPC* NPC = Cast<ATestNPC>(AICon ? AICon->GetPawn() : nullptr);
    if (!NPC || !AICon)
        return;

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
    if (!Target)
        return;

    float Distance = FVector::Dist(NPC->GetActorLocation(), Target->GetActorLocation());

    if (Distance > 450.f)
    {
        // 추격 포기 → 감지 초기화 + 정찰 상태로 복귀
        Blackboard->ClearValue("Target");
        Blackboard->SetValueAsBool(TEXT("bShouldChase"), false);
        NPC->SetShouldChase(false);

        AICon->StopMovement();
    }
}
