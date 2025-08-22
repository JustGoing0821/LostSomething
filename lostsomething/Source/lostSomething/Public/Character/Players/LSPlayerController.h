// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Character/UI/LSDeathWidget.h" 
#include "Interaction/LSInteractionEnum.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "Interface/LS2DPuzzleControllerInterface.h"
#include "LSPlayerController.generated.h"


UCLASS()
class LOSTSOMETHING_API ALSPlayerController : public APlayerController, public ILSCharacterChoiceInterface, public ILSScriptWidgetInterface, public ILSSiJaeCursorDragInterface, public ILS2DPuzzleControllerInterface
{
	GENERATED_BODY()

public:
	ALSPlayerController();
	//수정 게터함수
	FORCEINLINE ULSHUDWidget* GetLSHUDWidget() const { return LSHUDWidget; }

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	//void SelectNextSlot();

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	//void SelectPreviousSlot();


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


// HUD Section
protected:
	UPROPERTY()
	ULSHUDWidget* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSHUDWidget> LSHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSHUDWidget> LSHUDWidget;


//Death Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSDeathWidget> DeathWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSDeathWidget> DeathWidget;

public:
	// Death Widget 관리 함수들
	UFUNCTION(BlueprintCallable, Category = "Death")
	void ShowDeathWidget();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void HideDeathWidget();


//Script Section
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULSScriptWidget> ScriptWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULSScriptWidget> ScriptWidget;

public:
	virtual 	void UpdateScriptWidget(const FString& ScriptText) override;

	void StartTalking();

	void StopTalking();


// Quest Widget
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSQuestWidget> QuestWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSQuestWidget> QuestWidget;

public:
	void UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum);


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


// 2DPuzzle Widget
public:
	void Start2DPuzzle();
	void End2DPuzzle();
	void Update2DPuzzleTimer(float Timer);
	virtual void OnExit2DPuzzle() override;
	virtual void OnClear2DPuzzle() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULS2DPuzzleHUD> LS2DPuzzleHUDClass;

	UPROPERTY()
	TObjectPtr<class ULS2DPuzzleHUD> LS2DPuzzleHUDWidget;

	uint8 bIs2DPuzzleActive : 1;


// Aim Section
public:
	void UpdateAim(const FString& InString);


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
	void MulticastRPCStart2DPuzzle();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCEnd2DPuzzle();

	UFUNCTION(Server, Unreliable)
	void ServerRPCOnExit2DPuzzle();

	UFUNCTION(Server, Unreliable)
	void ServerRPCOnClear2DPuzzle();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCUpdate2DPuzzleTimer(float Timer);
};