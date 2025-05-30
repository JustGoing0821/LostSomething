// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "LSPlayerController.generated.h"


UCLASS()
class LOSTSOMETHING_API ALSPlayerController : public APlayerController, public ILSCharacterChoiceInterface
{
	GENERATED_BODY()

public:
	ALSPlayerController();
	//수정 게터함수
	FORCEINLINE ULSHUDWidget* GetLSHUDWidget() const { return LSHUDWidget; }



protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


//CharacterChoice Section
public:
	virtual ELSCharacterChoice GetCharacterChoice() override;
	virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override;

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

protected:
	UPROPERTY()
	class ULSScriptWidget* ScriptWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULSScriptWidget> ScriptWidgetClass;

public:
	void ShowScript(const FString& ScriptText);




};