// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSScriptWidget.h"
#include "lostSomething.h"
#include "Components/TextBlock.h"

ULSScriptWidget::ULSScriptWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULSScriptWidget::NativeConstruct()
{
    Super::NativeConstruct();

	Script = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtScript")));
	ensure(Script);

	//LS_WDGLOG(LogLS, Log, TEXT("Begin"));
}

void ULSScriptWidget::UpdateScriptWidget(const FString& InScript)
{
	//LS_WDGLOG(LogLS, Log, TEXT("Widget received: %s"), *InScript);
	//UE_LOG(LogLSWidget, Log, TEXT("Widget received length: %d"), InScript.Len());

	if (Script)
	{
		Script->SetText(FText::FromString(InScript));
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(ScriptTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ScriptTimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(ScriptTimerHandle, this, &ULSScriptWidget::ClearScriptWidget, 1.f, false, 2.f);
}

void ULSScriptWidget::ClearScriptWidget()
{
	Script->SetText(FText::FromString(""));
}