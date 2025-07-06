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

    UE_LOG(LogTemp, Warning, TEXT("=== IJae Attack START ==="));

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