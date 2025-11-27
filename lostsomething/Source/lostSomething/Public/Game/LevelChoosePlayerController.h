// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LevelChoosePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALevelChoosePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALevelChoosePlayerController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULevelChooseWidget> LevelChooseWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULevelChooseWidget> LevelChooseWidget;

};
