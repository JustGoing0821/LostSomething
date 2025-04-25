// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelTest/Player/LTPlayerCharacter.h"
#include "LTPlayerCharacterIJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerCharacterIJae : public ALTPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ALTPlayerCharacterIJae();

protected:
	virtual void PossessedBy(AController* NewController) override;
};
