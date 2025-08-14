// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LS2DPuzzleExitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULS2DPuzzleExitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILS2DPuzzleExitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnBtnExitClicked()=0;
};

//#include "Interface/LS2DPuzzleExitInterface.h"
