// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSFindingPuzzleManager.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSFindingPuzzleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALSFindingPuzzleManager();

protected:
	virtual void BeginPlay() override;

// Puzzle Section
protected:
	void PuzzleStart();

	void PuzzleCheck(uint8 inIsPuzzleCorrect);

// Quest Section
protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void QuestClear();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;

};
