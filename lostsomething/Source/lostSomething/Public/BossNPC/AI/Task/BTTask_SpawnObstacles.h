// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SpawnObstacles.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UBTTask_SpawnObstacles : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_SpawnObstacles();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    int32 CurrentSpawnCount;
    float TimeSinceLastSpawn;
    TWeakObjectPtr<AActor> ControlledPawn;
	
};
