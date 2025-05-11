// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectChaseDist.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UBTService_DetectChaseDist : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_DetectChaseDist();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
