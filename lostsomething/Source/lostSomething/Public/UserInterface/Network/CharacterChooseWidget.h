// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Players/LSCharacterChoice.h"
#include "CharacterChooseWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnCharacterChooseDelegate, ELSCharacterChoice InCharacterChoice);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UCharacterChooseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UCharacterChooseWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FOnCharacterChooseDelegate OnCharacterChoose;

	void UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice);

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtChoiceIJaeServer;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtChoiceSiJaeServer;
	
	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtChoiceIJaeClient;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtChoiceSiJaeClient;

	UPROPERTY()
	TObjectPtr<class UButton> BtnSiJae;

	UPROPERTY()
	TObjectPtr<class UButton> BtnIJae;

	UPROPERTY()
	TObjectPtr<class UButton> BtnGoLobby;

	UFUNCTION()
	void OnClickedBthSiJae();

	UFUNCTION()
	void OnClickedBthIJae();

	UFUNCTION()
	void OnClickedBtnGoLobby();

};
