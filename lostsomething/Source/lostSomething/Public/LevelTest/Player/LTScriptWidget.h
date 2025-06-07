// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LTScriptWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLSWidgetLT, Log, All);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULTScriptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULTScriptWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateScriptWidget(const FString& InScript);
	void ClearScriptWidget();

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> Script;

	FTimerHandle ScriptTimerHandle;
};
