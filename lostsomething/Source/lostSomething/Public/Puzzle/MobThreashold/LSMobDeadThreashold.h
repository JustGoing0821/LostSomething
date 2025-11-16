// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionTrigger.h"
#include "LSMobDeadThreashold.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSMobDeadThreashold : public ALSInteractionTrigger
{
	GENERATED_BODY()
	
public:
	ALSMobDeadThreashold();

	void CountDeadMobs();

protected:
	virtual void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum) override;

protected:
	UPROPERTY(EditAnywhere, Category=LSCustom)
	int32 TargetKilledMobCount;

	int32 CurrentKilledMobCount;

	UPROPERTY(EditAnywhere, Category = LSCustom)
	ELSInteractionEnum MobCleanEnum;

	void CleanAllMobs();
};
