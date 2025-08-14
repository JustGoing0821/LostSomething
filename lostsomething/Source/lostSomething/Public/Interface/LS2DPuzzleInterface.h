// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LS2DPuzzleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULS2DPuzzleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILS2DPuzzleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Start2DPuzzle(FName InPuzzleName, uint8 InIsStartTogether, APlayerController* InPlayerController) = 0;
	virtual void End2DPuzzle(FName InPuzzleName, uint8 InIsEndTogether, APlayerController* InPlayerController) = 0;
};

//#include "Interface/LS2DPuzzleInterface.h"
