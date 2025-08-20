// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Puzzle/UI/LS2DPuzzleDelegate.h"
#include "LS2DPuzzleGameModeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULS2DPuzzleGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILS2DPuzzleGameModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Start2DPuzzle(float Timer)=0;
	virtual void End2DPuzzle()=0;
	virtual void OnClear2DPuzzle() = 0;

	virtual FOn2DPuzzleClearDelegate& Get2DPuzzleClearDelegate() = 0;
	virtual void OnFailed2DPuzzle()=0;
	virtual FOn2DPuzzleFailedDelegate& Get2DPuzzleFailedDelegate() = 0;

};

//#include "Puzzle/UI/LS2DPuzzleDelegate.h"
//#include "Interface/LS2DPuzzleGameModeInterface.h"
