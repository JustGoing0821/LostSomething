// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TestNPCAnimIns.generated.h"


class ATestNPC;
UCLASS()
class LOSTSOMETHING_API UTestNPCAnimIns : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTestNPCAnimIns();

	float MontagePlay(UAnimMontage* Montage);
	void MontageStop(UAnimMontage* Montage, bool bInterrupted);

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AttackMontagePlay();

	UFUNCTION()
	void DamageMontagePlay();

	UFUNCTION()
	void DespawnMontagePlay();

	void JumpToAttackMontageSection(int32 NewSection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATestNPC* NPCCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DespawnMontage;

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
	void AnimNotify_Despawn();

	UFUNCTION()
	void AnimNotify_DamageEnd();

	UFUNCTION()
	void AnimNotify_IdleSound();
	UFUNCTION()
	void AnimNotify_DamageSound();
	UFUNCTION()
	void AnimNotify_WalkSound();
	UFUNCTION()
	void AnimNotify_RunSound();

	FName GetAttackMontageSectionName(int32 Section);
	
};
