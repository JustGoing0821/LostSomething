// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LSInformationWidget.h"
#include "Components/TextBlock.h"

ULSInformationWidget::ULSInformationWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULSInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TxtInfo = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_info")));
	ensure(TxtInfo);
}

void ULSInformationWidget::UpdateInfo(const FString& Script)
{
	TxtInfo->SetText(FText::FromString(FString::Printf(TEXT("%s"), *Script)));
}
