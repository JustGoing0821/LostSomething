// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSDeathWidget.h"
#include "Components/TextBlock.h"

void ULSDeathWidget::UpdateCountdown(int32 Count)
{
    if (CountdownText)
    {
        FString CountText = FString::Printf(TEXT("%d"), Count);
        CountdownText->SetText(FText::FromString(CountText));

        //UE_LOG(LogTemp, Warning, TEXT("Death countdown: %d"), Count);
    }
}
