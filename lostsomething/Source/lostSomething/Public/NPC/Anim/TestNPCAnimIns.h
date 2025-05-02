// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TestNPCAnimIns.generated.h"


UCLASS()
class LOSTSOMETHING_API UTestNPCAnimIns : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTestNPCAnimIns();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AttackMontagePlay();

	UFUNCTION()
	void DamageMontagePlay();

	void JumpToAttackMontageSection(int32 NewSection);

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bShouldChase = false;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UFUNCTION()
	void AnimNotify_EndAttack();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	UFUNCTION()
	void AnimNotify_EndDamage();

	FName GetAttackMontageSectionName(int32 Section);
	
};
