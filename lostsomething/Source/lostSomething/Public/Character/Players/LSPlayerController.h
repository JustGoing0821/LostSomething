// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interaction/LSInteractionEnum.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "LSPlayerController.generated.h"


UCLASS()
class LOSTSOMETHING_API ALSPlayerController : public APlayerController, public ILSCharacterChoiceInterface, public ILSScriptWidgetInterface
{
	GENERATED_BODY()

public:
	ALSPlayerController();
	//수정 게터함수
	FORCEINLINE ULSHUDWidget* GetLSHUDWidget() const { return LSHUDWidget; }

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	//void SelectNextSlot();

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	//void SelectPreviousSlot();


protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


//CharacterChoice Section
public:
	FORCEINLINE virtual ELSCharacterChoice GetCharacterChoice() override { return CharacterChoice; }
	FORCEINLINE virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override { CharacterChoice = InCharacterChoice; }

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;


// HUD Section
protected:
	UPROPERTY()
	ULSHUDWidget* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSHUDWidget> LSHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSHUDWidget> LSHUDWidget;


//Script Section
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULSScriptWidget> ScriptWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULSScriptWidget> ScriptWidget;

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