// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerSiJae.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSDarkWidget.h"
#include "GameFramework/CharacterMovementComponent.h"


void ALSPlayerSiJae::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled())  // 로컬 컨트롤러만 위젯을 띄움
    {
        if (DarkWidgetClass)
        {
            DarkWidgetInstance = CreateWidget<ULSDarkWidget>(GetWorld(), DarkWidgetClass);

            if (DarkWidgetInstance)
            {
                DarkWidgetInstance->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("DarkWidget successfully added to viewport"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DarkWidgetClass is not set in ALSPlayerSiJae"));
        }
    }
}


bool ALSPlayerSiJae::CanPushWheelchair() const
{
    return true;
}

void ALSPlayerSiJae::Attack() {
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

    if (HasAuthority())
    {
        ProcessAttack();
    }
    else
    {
        ServerProcessAttack();
    }
}