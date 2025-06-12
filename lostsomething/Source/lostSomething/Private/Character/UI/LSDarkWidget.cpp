// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSDarkWidget.h"

void ULSDarkWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기 상태는 완전 투명 (밝음)
    if (DarknessOverlay)
    {
        SetDarknessLevel(0.0f);
        UE_LOG(LogTemp, Warning, TEXT("DarkWidget: Initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DarkWidget: DarknessOverlay not found! Check widget binding."));
    }
}

void ULSDarkWidget::SetDarknessLevel(float DarknessAlpha)
{
    CurrentDarknessLevel = FMath::Clamp(DarknessAlpha, 0.0f, 1.0f);

    if (DarknessOverlay)
    {
        FLinearColor DarknessColor = FLinearColor(0.0f, 0.0f, 0.0f, CurrentDarknessLevel);
        DarknessOverlay->SetColorAndOpacity(DarknessColor);

        UE_LOG(LogTemp, Log, TEXT("DarkWidget: Darkness level set to: %f"), CurrentDarknessLevel);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DarkWidget: DarknessOverlay is null!"));
    }
}