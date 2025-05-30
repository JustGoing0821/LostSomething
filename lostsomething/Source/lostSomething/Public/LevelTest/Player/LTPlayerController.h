// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interaction/LSInteractionEnum.h"
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

protected:
	virtual void BeginPlay() override;

//CharacterChoice Section
public:
	virtual ELSCharacterChoice GetCharacterChoice() override;
	virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override;

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


// Quest Widget
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSQuestWidget> QuestWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSQuestWidget> QuestWidget;

	void UpdateQuestWidget(struct FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum);
};
