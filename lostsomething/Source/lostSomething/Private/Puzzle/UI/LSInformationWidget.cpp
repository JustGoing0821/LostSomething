// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LSInformationWidget.h"
#include "Level/LSInformationData.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

ULSInformationWidget::ULSInformationWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULSInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TxtInfo = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_info")));
	ensure(TxtInfo);

	InformationImage = Cast<UImage>(GetWidgetFromName(TEXT("Img_info")));
	ensure(InformationImage);
}

void ULSInformationWidget::UpdateInfo(const class ULSInformationData& InInformationData)
{
	TxtInfo->SetText(FText::FromString(FString::Printf(TEXT("%s"), *InInformationData.GetInformationScript())));
	InformationImage->SetBrushFromTexture(InInformationData.GetInformationImage());
}
