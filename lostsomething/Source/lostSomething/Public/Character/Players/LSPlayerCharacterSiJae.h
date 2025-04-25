// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayerCharacter.h"
#include "LSPlayerCharacterSiJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerCharacterSiJae : public ALSPlayerCharacter
{
	GENERATED_BODY()

public:
	ALSPlayerCharacterSiJae();

protected:
	virtual void PossessedBy(AController* NewController) override;


	
};
