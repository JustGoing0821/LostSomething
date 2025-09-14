// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSChatWidget.generated.h"

class UScrollBox;
class UEditableTextBox;


UCLASS()
class LOSTSOMETHING_API ULSChatWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
    virtual void NativeConstruct() override;

    /** [Sender] Text 형태로 한 줄 추가 + 자동 스크롤 다운 */
    UFUNCTION(BlueprintCallable)
    void AddChatLine(const FString& Sender, const FString& Text);

    //입력창 포거스
    UFUNCTION(BlueprintCallable)
    void FocusInput();

   //내용 비우기 입력창
    UFUNCTION(BlueprintCallable)
    void ClearInput();

    DECLARE_DELEGATE_OneParam(FOnChatCommitted, const FString&);
    FOnChatCommitted OnChatCommitted;

protected:
   
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ChatScroll = nullptr;

    
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* InputBox = nullptr;

   
    UFUNCTION()
    void OnInputCommitted(const FText& Text, ETextCommit::Type CommitType);

};
