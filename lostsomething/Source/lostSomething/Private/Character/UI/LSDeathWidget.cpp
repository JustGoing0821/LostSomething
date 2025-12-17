// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/UI/LSDeathWidget.h"
#include "Components/TextBlock.h"

void ULSDeathWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RoundBar)
    {
        UFunction* SetPercentFunc = RoundBar->FindFunction(FName("SetPercent"));
        if (SetPercentFunc)
        {
            float InitPercent = 0.0f;
            RoundBar->ProcessEvent(SetPercentFunc, &InitPercent);
        }
    }
}

void ULSDeathWidget::UpdateRespawnProgress(float RemainingTime, float TotalTime)
{
    if (CountdownText)
    {
        int32 Count = FMath::CeilToInt(RemainingTime);
        Count = FMath::Max(0, Count);
        CountdownText->SetText(FText::AsNumber(Count));
    }

    if (RoundBar)
    {
        float Progress = 1.0f - (RemainingTime / TotalTime);
        Progress = FMath::Clamp(Progress, 0.0f, 1.0f);

        UFunction* SetPercentFunc = RoundBar->FindFunction(FName("SetPercent"));
        if (SetPercentFunc)
        {
            RoundBar->ProcessEvent(SetPercentFunc, &Progress);
        }
    }
}