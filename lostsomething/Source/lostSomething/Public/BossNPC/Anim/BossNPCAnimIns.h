// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossNPCAnimIns.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UBossNPCAnimIns : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AOEMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* ObstacleMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* MazeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DamageMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	float MontagePlay(UAnimMontage* Montage);

	FName GetMontageSectionName(int32 Section);

	void MontageStop(UAnimMontage* Montage, bool bInterrupted);

	//UAnimMontage* CurrentMontage;

	UFUNCTION()
	void AnimNotify_AOEAttack();

	UFUNCTION()
	void AnimNotify_ObsAttack();

	UFUNCTION()
	void AnimNotify_MazeAttack();

	UFUNCTION()
	void AnimNotify_NextTiming();

	UFUNCTION()
	void AnimNotify_DeathMoment();

	UFUNCTION()
	void AnimNotify_DamageSound();

	UFUNCTION()
	void AnimNotify_DeathSound();
};
