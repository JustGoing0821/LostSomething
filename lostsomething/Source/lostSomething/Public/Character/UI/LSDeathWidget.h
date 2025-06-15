// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LSDeathWidget.generated.h"


UCLASS()
class LOSTSOMETHING_API ULSDeathWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 카운트다운 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCountdown(int32 Count);

protected:
    // 카운트다운 텍스트
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CountdownText;
};