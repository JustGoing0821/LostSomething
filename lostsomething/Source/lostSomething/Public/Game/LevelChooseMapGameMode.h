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

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void MoveToCharacterSelect();

protected:
	virtual void BeginPlay() override;

private:
	void CheckAllPlayersReady_Level();

	UPROPERTY()
	int32 RequiredPlayerCount = 2;
};
