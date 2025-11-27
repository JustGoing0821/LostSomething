// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LevelChooseMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALevelChooseMapGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	ALevelChooseMapGameMode();

public:

	UFUNCTION(BlueprintCallable, Category = "Game")
	void MoveToCharacterSelect();

protected:
	virtual void BeginPlay() override;

};
