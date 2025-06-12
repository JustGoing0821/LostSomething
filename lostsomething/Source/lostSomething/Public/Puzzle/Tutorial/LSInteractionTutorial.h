// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSInteractionTutorial.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSInteractionTutorial : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSInteractionTutorial();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//Quest Section
protected:
	void PuzzleCheck(bool isPuzzleCorrect);
	void QuestClear();

//RPC Section
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCQuestClear();
};
