// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interaction/LSInteractionEnum.h"
#include "Puzzle/UI/LS2DPuzzleDelegate.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "Interface/LS2DPuzzleControllerInterface.h"
#include "LTPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerController : public APlayerController, public ILSCharacterChoiceInterface, public ILSScriptWidgetInterface, public ILSSiJaeCursorDragInterface, public ILS2DPuzzleControllerInterface
{
	GENERATED_BODY()
	
public:
	ALTPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//CharacterChoice Section
public:
	FORCEINLINE virtual ELSCharacterChoice GetCharacterChoice() override { return CharacterChoice; }
	FORCEINLINE virtual void SetCharacterChoice(ELSCharacterChoice InCharacterChoice) override { CharacterChoice = InCharacterChoice; }

protected:
	UPROPERTY(EditAnywhere, Replicated)
	ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;


//Script Test
public:
	UFUNCTION(BlueprintCallable, Category = "Script Widget")
	virtual 	void UpdateScriptWidget(const FString& ScriptText) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULTScriptWidget> ScriptWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULTScriptWidget> ScriptWidget;


// Quest Widget
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSQuestWidget> QuestWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSQuestWidget> QuestWidget;

public:
	void UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum);


// 2DPuzzle Widget
public:
	void Start2DPuzzle(const FName& InWidgetName);
	void End2DPuzzle();
	void Update2DPuzzleTimer(float Timer);
	virtual void OnExit2DPuzzle(uint8 InIsExitTogether) override;
	virtual void OnClear2DPuzzle() override;




protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULS2DPuzzleHUD> LS2DPuzzleHUDClass;

	UPROPERTY()
	TObjectPtr<class ULS2DPuzzleHUD> LS2DPuzzleHUDWidget;

	//UPROPERTY(Replicated)
	uint8 bIs2DPuzzleActive : 1;


// SiJaeCursor Section
public:
	virtual void OnChangeSiJaeDragState(uint8 InIsSiJaeDragging) override;
	void CalledOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

	UPROPERTY(Replicated)
	FVector2D SiJaeCursorPos;

protected:
	void GetSiJaeLocalCursor();
	void SetGameModeSiJaeCursor(const FVector2D& InSiJaeCursorPos);
	void SendOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

//RPC
public:
	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateQuestWidget(FLSQuestData InQuestData);

	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateScriptWidget(const FString& ScriptText);

	UFUNCTION(Server, Unreliable)
	void ServerRPCSetGameModeSiJaeCursor(const FVector2D& InSiJaeCursorPos);

	UFUNCTION(Server, Unreliable)
	void ServerRPCSendOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

	UFUNCTION(Client, Unreliable)
	void ClientRPCCalledOnChangeSiJaeDragState(uint8 InIsSiJaeDragging);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCStart2DPuzzle(const FName& InWidgetName);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCEnd2DPuzzle();

	UFUNCTION(Server, Unreliable)
	void ServerRPCOnExit2DPuzzle();

	UFUNCTION(Server, Unreliable)
	void ServerRPCOnClear2DPuzzle();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCUpdate2DPuzzleTimer(float Timer);
};
