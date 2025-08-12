// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "LS2DPuzzleController.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALS2DPuzzleController : public APlayerController, public ILSCharacterChoiceInterface, public ILSSiJaeCursorDragInterface
{
	GENERATED_BODY()
	
public:
	ALS2DPuzzleController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


// CharacterChoice Section
public:
	FORCEINLINE virtual ELSCharacterChoice GetCharacterChoice() override { return CharacterChoice; }
	FORCEINLINE virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override { CharacterChoice = InCharacterChoice; }

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;


// Widget Section
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULS2DPuzzleHUD> LS2DPuzzleHUDClass;

	UPROPERTY()
	TObjectPtr<class ULS2DPuzzleHUD> LS2DPuzzleHUDWidget;


// SiJaeCursor Section
public:
	virtual void OnChangeSiJaeDragState(uint8 InIsSiJaeDragging) override;
	void CalledOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

protected:
	void GetSiJaeLocalCursor();
	void SetGameModeSiJaeCursor(const FVector2D& InSiJaeCursorPos);
	void SendOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

	UPROPERTY(Replicated)
	FVector2D SiJaeCursorPos;

// RPC
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCSetGameModeSiJaeCursor(const FVector2D& InSiJaeCursorPos);

	UFUNCTION(Server, Unreliable)
	void ServerRPCSendOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

	UFUNCTION(Client, Unreliable)
	void ClientRPCCalledOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);
};
