// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelTest/Player/LTPlayerCharacter.h"
#include "LTPlayerCharacterSiJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerCharacterSiJae : public ALTPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ALTPlayerCharacterSiJae();

protected:
	virtual void PossessedBy(AController* NewController) override;
};
