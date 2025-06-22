// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AOE.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UBTTask_AOE : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AOE();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	TWeakObjectPtr<AActor> ControlledPawn;

};
