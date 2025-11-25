// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelChooseWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULevelChooseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class ULSGameInstance* GI;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ChooseNewStart;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ChooseTutorial;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ChooseStage1;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ChooseStage2;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ChooseStage3;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnMyClicked_ChooseNewStart();

	UFUNCTION(BlueprintCallable)
	void OnMyClicked_ChooseTutorial();

	UFUNCTION(BlueprintCallable)
	void OnMyClicked_ChooseStage1();

	UFUNCTION(BlueprintCallable)
	void OnMyClicked_ChooseStage2();

	UFUNCTION(BlueprintCallable)
	void OnMyClicked_ChooseStage3();
};
