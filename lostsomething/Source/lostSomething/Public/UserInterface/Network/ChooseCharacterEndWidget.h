// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChooseCharacterEndWidget.generated.h"

DECLARE_DELEGATE(FOnCharacterChooseEndDelegate);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UChooseCharacterEndWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UChooseCharacterEndWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FOnCharacterChooseEndDelegate OnCharacterChooseEnd;

protected:
	UPROPERTY()
	TObjectPtr<class UButton> BtnGameStart;

	UFUNCTION()
	void OnClickedBtnGameStart();

	
};
