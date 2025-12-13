// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LevelChoosePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALevelChoosePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALevelChoosePlayerController();

	UFUNCTION()
	void CreateLevelChooseWidget();

	UFUNCTION(Server, Reliable)
	void Server_NotifyWidgetReady(); // 위젯 -> 서버 (보고)

	UFUNCTION(Client, Reliable)
	void Client_UnlockButton();
	
	UPROPERTY()
	class ULevelChooseWidget* LevelWidgetInstance;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_CreateLevelChooseWidget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UUserWidget> LevelChooseWidget;

};
