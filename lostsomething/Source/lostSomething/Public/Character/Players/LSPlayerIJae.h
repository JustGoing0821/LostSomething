// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayer.h"
#include "LSPlayerIJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerIJae : public ALSPlayer
{
	GENERATED_BODY()
	
protected:
	virtual bool CanPushWheelchair() const override;

	virtual void Jump() override;

	virtual void Attack() override;

	virtual void Move(const FInputActionValue& Value) override;

	void HandleWheelchairInput(const FVector2D& MovementVector);

public:
	// 휠체어 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheelchair Settings")
	float WheelchairTurnRate = 90.0f;

	// 회전 시 전진 이동 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheelchair Settings")
	float TurnMovementScale = 0.3f;
};