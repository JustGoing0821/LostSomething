// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerIJae.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

ALSPlayerIJae::ALSPlayerIJae()
{
    bWasBeingPushedLastFrame = false;
    StoredPusherRotation = FRotator::ZeroRotator;
    WheelchairTurnRate = 50.0f;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 1800.0f, 0.0f);
}

void ALSPlayerIJae::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 합체 중 시제 방향 저장
    if (bIsBeingPushed && IsValid(PusherCharacter))
    {
        StoredPusherRotation = PusherCharacter->GetActorRotation();
    }

    // 합체 해제 감지
    if (bIsBeingPushed != bWasBeingPushedLastFrame)
    {
        if (!bIsBeingPushed && Controller) // 합체 해제됨
        {
            // 카메라와 캐릭터를 동시에 즉시 회전
            FRotator NewRotation = StoredPusherRotation;
            NewRotation.Pitch = Controller->GetControlRotation().Pitch; // 피치는 현재 유지
            NewRotation.Roll = 0.0f;

            // 카메라 즉시 회전
            Controller->SetControlRotation(NewRotation);

            // 캐릭터 몸도 즉시 회전 (버벅임 방지)
            FRotator ActorRotation = NewRotation;
            ActorRotation.Pitch = 0.0f; // 캐릭터는 피치 0
            SetActorRotation(ActorRotation);

            UE_LOG(LogTemp, Warning, TEXT("IJae instantly rotated to: %.1f"), NewRotation.Yaw);
        }
        bWasBeingPushedLastFrame = bIsBeingPushed;
    }
}

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
