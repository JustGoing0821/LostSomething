// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSChatWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void ULSChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 입력창에서 Enter를 치면 OnInputCommitted가 호출
    if (InputBox)
    {
        InputBox->OnTextCommitted.AddDynamic(this, &ULSChatWidget::OnInputCommitted);
    }
}

void ULSChatWidget::AddChatLine(const FString& Sender, const FString& Text)
{
    if (!ChatScroll) return;

    // 한 줄 텍스트블록 생성
    UTextBlock* Line = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
    Line->SetText(FText::FromString(FString::Printf(TEXT("[%s] %s"), *Sender, *Text)));

    // ScrollBox에 추가하고 맨 아래로 스크롤
    ChatScroll->AddChild(Line);
    ChatScroll->ScrollToEnd();
}

void ULSChatWidget::FocusInput()
{
    if (InputBox)
    {
        InputBox->SetKeyboardFocus();
    }
}

void ULSChatWidget::ClearInput()
{
    if (InputBox)
    {
        InputBox->SetText(FText::GetEmpty());
    }
}

void ULSChatWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitType)
{
   
    if (CommitType == ETextCommit::OnEnter)
    {
        const FString Clean = Text.ToString().TrimStartAndEnd();

        if (OnChatCommitted.IsBound())
        {
            OnChatCommitted.Execute(Clean);
        }

        
        ClearInput();
        FocusInput();
    }
}
