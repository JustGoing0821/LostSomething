// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LSLocationTransferInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULSLocationTransferInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTSOMETHING_API ILSLocationTransferInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TransferPlayerLocation(FVector InSijaeLocation, FVector InIjaeLocation)=0;

};
