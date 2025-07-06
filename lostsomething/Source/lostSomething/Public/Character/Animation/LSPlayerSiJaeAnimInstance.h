// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LSPlayerSiJaeAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSPlayerSiJaeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	ULSPlayerSiJaeAnimInstance();
	void SetDeadAnim();
	void SetPickUpAnim();

protected:
	//딱 한번 호출. 초기화.
	virtual void NativeInitializeAnimation() override;

	//프레임마다 호출, 캐릭터의 현재 상태 업데이트
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* PickUpMontage;

	//왜 bp에서는 읽기만? 객체 타입선언 Owner 변수추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	//Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	//속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	//땅에서 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	//Idle 인지 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1; //비트 플래그 추가해야함

	//움직이고 있는가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshould;

	//떨어지는 중
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	//점프
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;

	//점프 중
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshould;
	
};
