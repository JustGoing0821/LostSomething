// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WaitingMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API AWaitingMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AWaitingMapGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	void CheckAllPlayersReady();

	UPROPERTY()
	int32 RequiredPlayerCount = 2;
};
