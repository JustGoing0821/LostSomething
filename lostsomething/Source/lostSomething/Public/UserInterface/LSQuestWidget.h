// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/LSQuestData.h"
#include "LSQuestWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULSQuestWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateQuestWidget(struct FLSQuestData InQuestData);

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> QuestName;

	UPROPERTY()
	TObjectPtr<class UTextBlock> Description;
};
