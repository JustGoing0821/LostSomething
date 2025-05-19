// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSScriptWidget.h"
#include "lostSomething.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"

void ULSScriptWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ScriptText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ScriptText")));

    if (!ScriptText)
    {
        WidgetTree->RootWidget = nullptr;

        UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>();
        WidgetTree->RootWidget = RootCanvas;

        UImage* DialogBackground = WidgetTree->ConstructWidget<UImage>();
        DialogBackground->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));

        UCanvasPanelSlot* BGSlot = RootCanvas->AddChildToCanvas(DialogBackground);
        BGSlot->SetSize(FVector2D(1920.0f, 120.0f)); 
        BGSlot->SetPosition(FVector2D(0.0f, 960.0f)); 
        BGSlot->SetAnchors(FAnchors(0.0f, 1.0f, 1.0f, 1.0f)); 
        BGSlot->SetAlignment(FVector2D(0.5f, 1.0f));
        BGSlot->SetOffsets(FMargin(0.0f, -120.0f, 0.0f, 0.0f)); 

        ScriptText = WidgetTree->ConstructWidget<UTextBlock>();

        ScriptText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))); 

        ScriptText->SetShadowOffset(FVector2D(2.0f, 2.0f));
        ScriptText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));

        ScriptText->SetJustification(ETextJustify::Center);

        UCanvasPanelSlot* TextSlot = RootCanvas->AddChildToCanvas(ScriptText);
        TextSlot->SetSize(FVector2D(1800.0f, 100.0f));
        TextSlot->SetPosition(FVector2D(960.0f, 970.0f)); 
        TextSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f)); 
        TextSlot->SetAlignment(FVector2D(0.5f, 0.5f)); 
        TextSlot->SetOffsets(FMargin(0.0f, -60.0f, 0.0f, 0.0f));

        ScriptText->SetText(FText::FromString(TEXT("...")));
    }
    else
    {
        FSlateFontInfo FontInfo = ScriptText->GetFont();
        FontInfo.Size = 28.0f;
        ScriptText->SetFont(FontInfo);

        ScriptText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
        ScriptText->SetShadowOffset(FVector2D(2.0f, 2.0f));
        ScriptText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
        ScriptText->SetJustification(ETextJustify::Center);
    }

    SetVisibility(ESlateVisibility::Hidden);
}

void ULSScriptWidget::SetScriptText(const FString& NewText)
{
    if (ScriptText)
    {
        ScriptText->SetText(FText::FromString(NewText));
    }
    else
    {
    }
}

void ULSScriptWidget::ShowScriptWidget()
{
    SetVisibility(ESlateVisibility::HitTestInvisible);

    if (AutoHideDelay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoHideTimerHandle,
            this,
            &ULSScriptWidget::HideScriptWidget,
            AutoHideDelay,
            false
        );
    }
}

void ULSScriptWidget::HideScriptWidget()
{
    SetVisibility(ESlateVisibility::Hidden);

    if (GetWorld()->GetTimerManager().IsTimerActive(AutoHideTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoHideTimerHandle);
    }
}