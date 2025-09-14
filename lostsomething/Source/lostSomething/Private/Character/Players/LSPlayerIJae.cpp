// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerIJae.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

void ALSPlayerIJae::Jump()
{
    return;
}

void ALSPlayerIJae::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ALSPlayer::StartThrowPreview);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ALSPlayer::EndThrowPreview, true);


    }
}

void ALSPlayerIJae::Attack() {
    if (bIsDead) return;

    int32 CurrentSelectedSlot = SelectedSlot;
    if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
    {
        if (!ItemInfoArray[CurrentSelectedSlot].IsEmpty)
        {
            ThrowItem();
            return;
        }
    }
}

void ALSPlayerIJae::Move(const FInputActionValue& Value)
{
    if (bIsDead) 
        return;

    if (!IsLocallyControlled())
        return;

    if (bIsCombining)
        return;

    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        HandleWheelchairInput(MovementVector);
    }
}

void ALSPlayerIJae::HandleWheelchairInput(const FVector2D& MovementVector)
{
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    if (FMath::Abs(MovementVector.Y) > 0.1f)
    {
        AddMovementInput(ForwardDirection, MovementVector.Y);
    }

    // 좌우 입력을 회전 처리
    if (FMath::Abs(MovementVector.X) > 0.1f)
    {
        float TurnInput = MovementVector.X * (WheelchairTurnRate / 200.0f); // 값 조정
        AddControllerYawInput(TurnInput);
    }
}
