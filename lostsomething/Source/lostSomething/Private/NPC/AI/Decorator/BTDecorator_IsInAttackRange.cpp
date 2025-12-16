// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/Decorator/BTDecorator_IsInAttackRange.h"
#include "NPC/AI/TestNPCAIController.h"
#include "NPC/TestNPC.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return false;

	//Character
	// 여기도 시제 이제 중 하나로 타겟 잡아서 비교해야할듯
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATestNPCAIController::Key_Target));
	if (nullptr == Target) return false;

	bool bResult = (Target->GetDistanceTo(ControllingPawn) <= 180.0f);
	return bResult;
}
