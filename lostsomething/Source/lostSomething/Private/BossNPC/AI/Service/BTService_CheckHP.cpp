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
    UE_LOG(LogTemp, Error, TEXT("UBTService_CheckHP : %f"), CurrentHP);

    FName NewState;
    if (CurrentHP <= 30.0f)
        BlackboardComp->SetValueAsEnum(FName("Key_Phase"), static_cast<uint8>(EPhaseType::Phase3));
    else if (CurrentHP <= 70.0f)
        BlackboardComp->SetValueAsEnum(FName("Key_Phase"), static_cast<uint8>(EPhaseType::Phase2));
    else
        BlackboardComp->SetValueAsEnum(FName("Key_Phase"), static_cast<uint8>(EPhaseType::Phase1));

    
}
