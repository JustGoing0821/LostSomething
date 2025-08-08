// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "LS2DPuzzleController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALS2DPuzzleController : public APlayerController, public ILSCharacterChoiceInterface
{
	GENERATED_BODY()
	
public:
	ALS2DPuzzleController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	float SijaeMousePosX;

	UPROPERTY(Replicated)
	float SijaeMousePosY;

//CharacterChoice Section
public:
	FORCEINLINE virtual ELSCharacterChoice GetCharacterChoice() override { return CharacterChoice; }
	FORCEINLINE virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override { CharacterChoice = InCharacterChoice; }

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;


//RPC
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCSetMousePosition(float InMouseX, float InMouseY);
};
