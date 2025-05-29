// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"


void ULSHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("ULSHUDWidget::NativeConstruct() Called"));

}

void ULSHUDWidget::UpdateHpBar(float NewHp)
{
    if (PbHpBar)
    {
        const float Percent = NewHp / 100.0f;
        PbHpBar->SetPercent(Percent);
        UE_LOG(LogTemp, Warning, TEXT("hud : HpBar updated: %.1f%%"), Percent * 100.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HUD : PbHpBar is nullptr!"));
    }
}


