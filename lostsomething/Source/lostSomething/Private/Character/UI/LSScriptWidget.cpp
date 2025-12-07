// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSScriptWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"

DEFINE_LOG_CATEGORY(LogLSWidget);

ULSScriptWidget::ULSScriptWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULSScriptWidget::NativeConstruct()
{
    Super::NativeConstruct();

	Script = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtScript")));
	ensure(Script);

	ScriptGroup = Cast<UOverlay>(GetWidgetFromName(TEXT("ScriptGroup")));
	ensure(ScriptGroup);

	//UE_LOG(LogLSWidget, Log, TEXT("Begin"));
}

void ULSScriptWidget::UpdateScriptWidget(const FString& InScript)
{
	//UE_LOG(LogLSWidget, Log, TEXT("Widget received: %s"), *InScript);
	//UE_LOG(LogLSWidget, Log, TEXT("Widget received length: %d"), InScript.Len());

	if (Script)
	{
		Script->SetText(FText::FromString(InScript));
	}

	if ((InScript != "") && ScriptGroup)
	{
		ScriptGroup->SetVisibility(ESlateVisibility::Visible);
	}

	if ((InScript == "") && ScriptGroup)
	{
		ScriptGroup->SetVisibility(ESlateVisibility::Collapsed);
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

	if (ScriptGroup)
	{
		ScriptGroup->SetVisibility(ESlateVisibility::Collapsed);
	}
}