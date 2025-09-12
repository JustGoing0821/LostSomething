// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSHpWidget.h"


void ULSHpWidget::NativeConstruct()
{
    Super::NativeConstruct();
   // UE_LOG(LogTemp, Warning, TEXT("ULSHUDWidget::NativeConstruct() Called"));

}



void ULSHpWidget::UpdateHpBar(float NewHp)
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