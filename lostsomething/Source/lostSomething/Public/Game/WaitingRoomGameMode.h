// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WaitingRoomGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API AWaitingRoomGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AWaitingRoomGameMode();

protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UChooseCharacterEndWidget> GameStartWidgetClass;

	UPROPERTY()
	TObjectPtr<class UChooseCharacterEndWidget> GameStartWidget;

	void GameStart();

};

