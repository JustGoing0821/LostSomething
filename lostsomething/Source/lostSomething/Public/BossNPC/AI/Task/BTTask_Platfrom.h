// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Platfrom.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UBTTask_Platfrom : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Platfrom();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
