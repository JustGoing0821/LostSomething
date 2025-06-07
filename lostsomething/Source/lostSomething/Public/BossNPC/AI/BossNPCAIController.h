// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossNPCAIController.generated.h"


UENUM(BlueprintType)
enum class EPhaseType : uint8
{
    Phase1    UMETA(DisplayName = "Phase1"),
    Phase2    UMETA(DisplayName = "Phase2"),
    Phase3    UMETA(DisplayName = "Phase3"),
};

UCLASS()
class LOSTSOMETHING_API ABossNPCAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABossNPCAIController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	void StopAI();

	void ChangedHP();


	//BB Key º¯¼ö¸í
	static const FName Key_Phase;
	static const FName Key_CurrentHP;


private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;

	
};
