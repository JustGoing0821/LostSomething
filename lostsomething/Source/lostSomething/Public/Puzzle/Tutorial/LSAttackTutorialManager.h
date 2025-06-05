// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSAttackTutorialManager.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSAttackTutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSAttackTutorialManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void PuzzleCheckCall();
	void PuzzleCheck();
	void QuestClear();

	int32 CurrentClearCharacter;

//RPC
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCPuzzleCheck();

};
