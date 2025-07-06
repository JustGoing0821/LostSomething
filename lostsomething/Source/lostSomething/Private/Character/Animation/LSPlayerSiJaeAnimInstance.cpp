// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Animation/LSPlayerSiJaeAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


ULSPlayerSiJaeAnimInstance::ULSPlayerSiJaeAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
	
}

void ULSPlayerSiJaeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void ULSPlayerSiJaeAnimInstance::SetDeadAnim()
{
	UE_LOG(LogTemp, Warning, TEXT("SetDeadAnim called"));

	if (!DeathMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("DeathMontage is NULL!"));
		return;
	}

	if (!Montage_IsPlaying(DeathMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Playing Now"));
		Montage_Play(DeathMontage);
	}
}

void ULSPlayerSiJaeAnimInstance::SetPickUpAnim()
{
	UE_LOG(LogTemp, Warning, TEXT("PickUpdAnim called"));

	if (!PickUpMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("PickupMontage is NULL!"));
		return;
	}

	if (!Montage_IsPlaying(PickUpMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Playing Now"));
		Montage_Play(PickUpMontage);
	}
}

void ULSPlayerSiJaeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
	}
}
