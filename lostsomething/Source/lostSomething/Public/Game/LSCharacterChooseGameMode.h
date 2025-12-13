// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Game/LSNetworkPosition.h"
#include "LSCharacterChooseGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FCharacterChoiceChangedDelegate, ELSCharacterChoice /*ServerChoice*/, ELSCharacterChoice /*ClientChoice*/);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSCharacterChooseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALSCharacterChooseGameMode();

protected:
	virtual void BeginPlay() override;

public:
	FCharacterChoiceChangedDelegate CharacterChoiceChanged;

	void SetCharacterChoice(bool bisServer, ELSCharacterChoice InCharacterChoice);

	UPROPERTY()
	int32 PlayerCount = 0;

protected:
	TMap<ELSNetworkPosition, ELSCharacterChoice> CharacterChoices;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UChooseCharacterEndWidget> GameStartWidgetClass;

	UPROPERTY()
	TObjectPtr<class UChooseCharacterEndWidget> GameStartWidget;

	void GameStart();
	
};
