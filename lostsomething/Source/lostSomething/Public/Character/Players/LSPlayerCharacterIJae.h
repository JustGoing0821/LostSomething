// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayerCharacter.h"
#include "LSPlayerCharacterIJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerCharacterIJae : public ALSPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ALSPlayerCharacterIJae();

protected:
	virtual void PossessedBy(AController* NewController) override;
};
