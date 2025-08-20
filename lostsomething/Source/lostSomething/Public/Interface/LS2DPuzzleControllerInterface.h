// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Puzzle/UI/LS2DPuzzleDelegate.h"
#include "LS2DPuzzleControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULS2DPuzzleControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILS2DPuzzleControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
//Wiget Call
	virtual void OnExit2DPuzzle()=0;
	virtual void OnClear2DPuzzle()=0;
};


//#include "Interface/LS2DPuzzleControllerInterface.h"

