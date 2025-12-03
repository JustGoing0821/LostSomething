// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/LSQuestWidget.h"
#include "lostSomething.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"

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

	QuestClear = Cast<UImage>(GetWidgetFromName(TEXT("ImgQuestClear")));
	ensure(QuestClear);
	if (QuestClear)
	{
		QuestClear->SetVisibility(ESlateVisibility::Hidden);
	}

	QuestStart = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasQuestStart")));
	ensure(QuestStart);
	if (QuestStart)
	{
		QuestStart->SetVisibility(ESlateVisibility::Hidden);
	}

	QuestStartDescription = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtQuestStart")));
	ensure(QuestStartDescription);
}

void ULSQuestWidget::UpdateQuestWidget(FLSQuestData InQuestData)
{
	if (QuestName)
	{
		QuestName->SetText(FText::FromString(InQuestData.QuestName));
	}

	if (Description)
	{
		Description->SetText(FText::FromString(InQuestData.Description));
	}
}

void ULSQuestWidget::CallQuestClear()
{
	if (!QuestClear)
	{
		LS_WDGLOG(LogLSls, Error, TEXT("%s"), TEXT("No QuestClear!"));
		return;
	}

	QuestClear->SetVisibility(ESlateVisibility::Visible);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			QuestClear->SetVisibility(ESlateVisibility::Hidden);
		}
	), 1.0f, false);
}

void ULSQuestWidget::CallQuestStart(FLSQuestData InQuestData)
{
	if (QuestStartDescription)
	{
		QuestStartDescription->SetText(FText::FromString(InQuestData.QuestName));
	}

	if (!QuestStart)
	{
		LS_WDGLOG(LogLSls, Error, TEXT("%s"), TEXT("No QuestStart!"));
		return;
	}

	QuestStart->SetVisibility(ESlateVisibility::Visible);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			QuestStart->SetVisibility(ESlateVisibility::Hidden);
		}
	), 1.0f, false);
}
