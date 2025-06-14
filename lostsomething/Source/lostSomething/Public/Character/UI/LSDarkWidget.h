// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "LSDarkWidget.generated.h"

UCLASS()
class LOSTSOMETHING_API ULSDarkWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 어둠 정도 설정 (0.0 = 완전 밝음, 1.0 = 완전 어둠)
    UFUNCTION(BlueprintCallable, Category = "Dark")
    void SetDarknessLevel(float DarknessAlpha);

protected:
    // 어둠 이미지 (WBP_Dark에서 바인딩할 이름)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> DarknessOverlay;

    // 현재 어둠 레벨
    UPROPERTY(BlueprintReadOnly, Category = "Dark")
    float CurrentDarknessLevel = 0.0f;
};