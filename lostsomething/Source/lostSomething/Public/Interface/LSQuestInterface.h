// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LSQuestInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULSQuestInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILSQuestInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void QuestStart()=0;
	virtual void QuestComplete()=0;
	virtual class ALSQuestManager* GetQuestManager() = 0;
};

//#include "Interface/LSQuestInterface.h"

