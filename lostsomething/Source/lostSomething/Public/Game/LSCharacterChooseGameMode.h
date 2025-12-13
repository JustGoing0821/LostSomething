// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Game/LSNetworkPosition.h"
#include "LSCharacterChooseGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FCharacterChoiceChangedDelegate, ELSCharacterChoice /*ServerChoice*/, ELSCharacterChoice /*ClientChoice*/);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSCharacterChooseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALSCharacterChooseGameMode();

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	UPROPERTY()
	TArray<APlayerController*> LoggedInPlayers;

public:
	FCharacterChoiceChangedDelegate CharacterChoiceChanged;

	void SetCharacterChoice(bool bisServer, ELSCharacterChoice InCharacterChoice);

	UPROPERTY()
	FString ServerPlayerNickName;

	UPROPERTY()
	FString ClientPlayerNickName;

	// 닉네임 설정 (클라이언트가 서버에 요청)
	UFUNCTION(BlueprintCallable)
	void SetPlayerNickName(APlayerController* PC, const FString& NickName);

	// 모든 클라이언트에게 닉네임 전송
	UFUNCTION(BlueprintCallable)
	void BroadcastNickNames();

	UPROPERTY()
	int32 PlayerCount = 0;

protected:
	TMap<ELSNetworkPosition, ELSCharacterChoice> CharacterChoices;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UChooseCharacterEndWidget> GameStartWidgetClass;

	UPROPERTY()
	TObjectPtr<class UChooseCharacterEndWidget> GameStartWidget;

	void GameStart();
	
};
