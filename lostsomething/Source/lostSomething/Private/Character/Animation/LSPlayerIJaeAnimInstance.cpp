// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Animation/LSPlayerIJaeAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULSPlayerIJaeAnimInstance::ULSPlayerIJaeAnimInstance()
{
	MovingThreshould = 3.0f;
	//JumpingThreshould = 100.0f;
}

void ULSPlayerIJaeAnimInstance::SetDeadAnim()
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

void ULSPlayerIJaeAnimInstance::ThrowingAnim()
{
	UE_LOG(LogTemp, Warning, TEXT("ThrowingAnim called"));

	if (!ThrowingMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("ThrowingAnim is NULL!"));
		return;
	}

	if (!Montage_IsPlaying(ThrowingMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Playing Now"));
		Montage_Play(ThrowingMontage);
	}
}

void ULSPlayerIJaeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void ULSPlayerIJaeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		//bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
	}
}
