// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSDarkWidget.h"
#include "LSPlayerSiJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerSiJae : public ALSPlayer
{
	GENERATED_BODY()

	// LSPlayerSiJae.h

	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DarkWidgetClass;

	UPROPERTY()
	ULSDarkWidget* DarkWidgetInstance;
	
protected:
	virtual bool CanPushWheelchair() const override;
};