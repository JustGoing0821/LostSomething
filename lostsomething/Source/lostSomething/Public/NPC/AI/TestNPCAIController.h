// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TestNPCAIController.generated.h"

UCLASS()
class LOSTSOMETHING_API ATestNPCAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATestNPCAIController();
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	void StopAI();


	//BB Key º¯¼ö¸í
	static const FName Key_HomePos;
	static const FName Key_PatrolPos;
	static const FName Key_Target;
	static const FName Key_bShouldChase;

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
	
};
