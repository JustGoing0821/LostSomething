// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interaction/LSInteractionEnum.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "LTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerController : public APlayerController, public ILSCharacterChoiceInterface, public ILSScriptWidgetInterface
{
	GENERATED_BODY()
	
public:
	ALTPlayerController();

protected:
	virtual void BeginPlay() override;

//CharacterChoice Section
public:
	FORCEINLINE virtual ELSCharacterChoice GetCharacterChoice() override { return CharacterChoice; }
	FORCEINLINE virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override { CharacterChoice = InCharacterChoice; }

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


//Script Test
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULTScriptWidget> ScriptWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULTScriptWidget> ScriptWidget;

public:
	virtual 	void UpdateScriptWidget(const FString& ScriptText) override;



// Quest Widget
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSQuestWidget> QuestWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSQuestWidget> QuestWidget;

public:
	void UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum);

//RPC
public:
	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateQuestWidget(FLSQuestData InQuestData);

	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateScriptWidget(const FString& ScriptText);
};
