// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Puzzle/UI/LS2DPuzzleClearDelegate.h"
#include "LS2DPuzzleClearInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULS2DPuzzleClearInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILS2DPuzzleClearInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnClear2DPuzzle()=0;
	//virtual void BindTo2DPuzzleClearDelegate(UObject* InObject, FName FunctionName) = 0;
	virtual FOn2DPuzzleClearDelegate& Get2DPuzzleClearDelegate() = 0;

};

//#include "Interface/LS2DPuzzleClearInterface.h"
