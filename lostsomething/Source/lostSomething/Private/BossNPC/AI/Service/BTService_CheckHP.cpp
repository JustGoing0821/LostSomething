// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/Service/BTService_CheckHP.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <BossNPC/AI/BossNPCAIController.h>

UBTService_CheckHP::UBTService_CheckHP()
{
	NodeName = TEXT("Check HP and Set State");

	BlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckHP, BlackboardKey));
}

void UBTService_CheckHP::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return;

    float CurrentHP = BlackboardComp->GetValueAsFloat(FName("CurrentHP"));

    FName NewState;
    if (CurrentHP <= 80.0f)
    {
        BlackboardComp->SetValueAsEnum(FName("Phase"), static_cast<uint8>(EPhaseType::Phase3));
        //UE_LOG(LogTemp, Warning, TEXT("UBTService_CheckHP::EPhaseType::Phase3"))
    }     
    else if (CurrentHP <= 160.0f)
    {
        BlackboardComp->SetValueAsEnum(FName("Phase"), static_cast<uint8>(EPhaseType::Phase2));
        //UE_LOG(LogTemp, Warning, TEXT("UBTService_CheckHP::EPhaseType::Phase2"))
    }
    else
    {
        BlackboardComp->SetValueAsEnum(FName("Phase"), static_cast<uint8>(EPhaseType::Phase1));
        //UE_LOG(LogTemp, Warning, TEXT("UBTService_CheckHP::EPhaseType::Phase1"))
    }
}
