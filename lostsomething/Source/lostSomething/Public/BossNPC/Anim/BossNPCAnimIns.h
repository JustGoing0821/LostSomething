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

	void MontagePlay(UAnimMontage* Montage);

	void JumpToMontageSection(int32 NewSection, UAnimMontage* Montage);

	void PlayRandomMontageSection(UAnimMontage* Montage);

	FName GetMontageSectionName(int32 Section);

	UFUNCTION()
	void AnimNotify_AOEAttack();

	UFUNCTION()
	void AnimNotify_ObsAttack();

	UFUNCTION()
	void AnimNotify_MazeAttack();
};
