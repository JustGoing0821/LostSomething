// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerIJae.h"

bool ALSPlayerIJae::CanPushWheelchair() const
{
    return false;
}

void ALSPlayerIJae::Jump()
{
    return;
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
    if (bIsDead) return;

    if (!IsLocallyControlled())
        return;

    if (bIsBeingPushed)
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
        float TurnInput = MovementVector.X * (WheelchairTurnRate / 100.0f); // 값 조정
        AddControllerYawInput(TurnInput);

        UE_LOG(LogTemp, Warning, TEXT("Wheelchair turning: Input=%.2f, TurnRate=%.2f"),
            MovementVector.X, WheelchairTurnRate);
    }
}
