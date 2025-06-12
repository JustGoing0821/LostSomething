// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LSCombineTutorialInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULSCombineTutorialInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILSCombineTutorialInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool isCombining() = 0;
};

//#include "Interface/LSCombineTutorialInterface.h"
