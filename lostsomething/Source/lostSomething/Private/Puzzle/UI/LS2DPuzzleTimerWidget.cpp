// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DPuzzleTimerWidget.h"
#include "Components/TextBlock.h"

ULS2DPuzzleTimerWidget::ULS2DPuzzleTimerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULS2DPuzzleTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TxtTimer = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_timer")));
	ensure(TxtTimer);
}

void ULS2DPuzzleTimerWidget::UpdateTimer(float Timer)
{
	TxtTimer->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Timer)));
}
