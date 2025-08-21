// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LS2DPuzzleTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULS2DPuzzleTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULS2DPuzzleTimerWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtTimer;

public:
	void UpdateTimer(float Timer);
};
