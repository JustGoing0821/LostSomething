// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//=============================================
	//     PROPERTIES & VARIABLES
	//=============================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_Continue;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_Lobby;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ChooseMap;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_Quit;

	UPROPERTY()
	class ALSGameMode* GM;

	//=============================================
	//     FUCTIONS
	//=============================================
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;
	
	UFUNCTION(BlueprintCallable)
	void ContinueGame();

	UFUNCTION(BlueprintCallable)
	void GoLobby();

	UFUNCTION(BlueprintCallable)
	void GoChooseMap();

	UFUNCTION(BlueprintCallable)
	void QuitGame();

	UFUNCTION(BlueprintCallable)
	void MenuVisibility();
};
