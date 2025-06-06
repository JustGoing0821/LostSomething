// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayerCharacter.h"
#include "LSPlayerCharacterLeeJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerCharacterLeeJae : public ALSPlayerCharacter
{
	GENERATED_BODY()

	public:
	ALSPlayerCharacterLeeJae();

protected:
	virtual void PossessedBy(AController* NewController) override;
};
