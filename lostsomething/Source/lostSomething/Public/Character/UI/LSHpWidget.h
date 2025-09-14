// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "LSHpWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSHpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PbHpBar;

	// 바인딩용 함수
	void UpdateHpBar(float NewHp);

	
};
