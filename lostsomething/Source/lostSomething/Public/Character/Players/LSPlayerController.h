// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "LSPlayerController.generated.h"



UCLASS()
class LOSTSOMETHING_API ALSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALSPlayerController();

protected:
	virtual void BeginPlay() override;

	// HUD Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSHUDWidget> LSHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSHUDWidget> LSHUDWidget;

private:
	UPROPERTY()
	class ULSScriptWidget* ScriptWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULSScriptWidget> ScriptWidgetClass;

public:
	void ShowScript(const FString& ScriptText);
};