// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/UI/LSHpBarWidget.h"
#include "Interface/LSCharacterWidgetInterface.h"
#include "Components/ProgressBar.h"

//ULSHpBarWidget::ULSHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
//{
//	MaxHp = -1.0f;
//
//}
//
//void ULSHpBarWidget::NativeConstruct()
//{
//	Super::NativeConstruct();
//
//	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
//	ensure(HpProgressBar);
//
//	ILSCharacterWidgetInterface* CharacterWidget = Cast<ILSCharacterWidgetInterface>(OwningActor);
//	if (CharacterWidget)
//	{
//		CharacterWidget->SetupCharacterWidget(this);
//	}
//}
//
//void ULSHpBarWidget::UpdateHpBar(float NewCurrentHp)
//{
//	ensure(MaxHp > 0.0f);
//	if (HpProgressBar)
//	{
//		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
//	}
//}