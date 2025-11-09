// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "LSCharacterChoiceController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSCharacterChoiceController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALSCharacterChoiceController();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCharacterChooseWidget> CharacterChooseWidgetClass;

	UPROPERTY()
	TObjectPtr<class UCharacterChooseWidget> CharacterChooseWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UVRReadyWidget> VRReadyWidgetClass;

	UPROPERTY()
	TObjectPtr<class UVRReadyWidget> VRReadyWidget;

protected:
	virtual void BeginPlay() override;
	
	void OnCharacterChoose(ELSCharacterChoice InCharacterChoice);

	void SetCharacterChoice(bool bisServer, ELSCharacterChoice InCharacterChoice);

	void UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice);

public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCSetCharacterChoice(ELSCharacterChoice InCharacterChoice);

	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice);
};
