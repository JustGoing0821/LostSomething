// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTScriptWidget.h"
#include "Components/TextBlock.h"

DEFINE_LOG_CATEGORY(LogLSWidget);

ULTScriptWidget::ULTScriptWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULTScriptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Script = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtScript")));
	ensure(Script);

	//UE_LOG(LogLSWidget, Log, TEXT("Begin"));
}

void ULTScriptWidget::UpdateScriptWidget(const FString& InScript)
{
	//UE_LOG(LogLSWidget, Log, TEXT("Widget received: %s"), *InScript);
	//UE_LOG(LogLSWidget, Log, TEXT("Widget received length: %d"), InScript.Len());

	if (Script)
	{
		Script->SetText(FText::FromString(InScript));
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(ScriptTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ScriptTimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(ScriptTimerHandle, this, &ULTScriptWidget::ClearScriptWidget, 1.f, false, 2.f);
}

void ULTScriptWidget::ClearScriptWidget()
{
	Script->SetText(FText::FromString(""));
}
