// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/UI/LSUserWidget.h"
#include "LSHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSHpBarWidget : public ULSUserWidget
{
	GENERATED_BODY()


public:
	ULSHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
