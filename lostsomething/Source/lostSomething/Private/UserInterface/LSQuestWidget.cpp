// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/LSQuestWidget.h"
#include "lostSomething.h"
#include "Components/TextBlock.h"

ULSQuestWidget::ULSQuestWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULSQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	QuestName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtQuestName")));
	ensure(QuestName);

	Description = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtQuestDescription")));
	ensure(Description);
}

void ULSQuestWidget::UpdateQuestWidget(FLSQuestData InQuestData)
{
	//LS_WDGLOG(LogLS, Log, TEXT("Begin"));
	if (QuestName)
	{
		QuestName->SetText(FText::FromString(InQuestData.QuestName));
	}

	if (Description)
	{
		Description->SetText(FText::FromString(InQuestData.Description));
	}
}
