// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSInformationWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSInformationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULSInformationWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtInfo;

public:
	void UpdateInfo(const FString& Script);
};
