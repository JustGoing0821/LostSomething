// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "LTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerController : public APlayerController, public ILSCharacterChoiceInterface
{
	GENERATED_BODY()
	
public:
	ALTPlayerController();

//CharacterChoice Section
public:
	virtual ELSCharacterChoice GetCharacterChoice() override;
	virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override;

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
