// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionActor/LSInteractionActorBase.h"
#include "Puzzle/VendingMachine/VendingMachineColor.h"
#include "LSVendingMachineManager.generated.h"

UENUM(BlueprintType)
enum class ECurrentPhase : uint8
{
	Phase1 = 0,
	Phase2,
	Phase3,
	Phase4
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPhaseChangedDelegate, EVendingMachineColor /*AnswerColor*/, int32 /*CurrentColorSet*/);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSVendingMachineManager : public ALSInteractionActorBase
{
	GENERATED_BODY()
	
public:
	ALSVendingMachineManager();

protected:
	virtual void BeginPlay() override;


//Vending Machine Section
protected:
	TArray<class ALSVendingMachine*> VendingMachines;


//Puzzle Section
public:
	FOnPhaseChangedDelegate OnPhaseChanged;

protected:
	ECurrentPhase CurrentPhase;
	int32 CurrentColorSet;
	TArray<EVendingMachineColor> AnswerColors;

	void StartPhase();
	
};
