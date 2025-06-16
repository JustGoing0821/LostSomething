// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LSTakeDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULSTakeDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILSTakeDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) = 0;
};

//#include "Interface/LSTakeDamageInterface.h"