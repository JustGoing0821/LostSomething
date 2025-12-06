// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSScriptWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLSWidget, Log, All);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSScriptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULSScriptWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;
	
public:
	void UpdateScriptWidget(const FString& InScript);
	void ClearScriptWidget();

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> Script;

	UPROPERTY()
	TObjectPtr<class UOverlay> ScriptGroup;

	FTimerHandle ScriptTimerHandle;
};
