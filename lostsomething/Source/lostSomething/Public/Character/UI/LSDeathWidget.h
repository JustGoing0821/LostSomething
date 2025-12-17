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
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void UpdateRespawnProgress(float RemainingTime, float TotalTime);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CountdownText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUserWidget> RoundBar;
};