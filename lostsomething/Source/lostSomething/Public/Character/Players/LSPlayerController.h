// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interaction/LSInteractionEnum.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "Interface/LS2DPuzzleControllerInterface.h"
#include "Interface/LSStopKeyInputInterface.h"
#include "LSPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBGMStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBGMStopDelegate);


UCLASS()
class LOSTSOMETHING_API ALSPlayerController : public APlayerController, public ILSCharacterChoiceInterface, public ILSScriptWidgetInterface, public ILSSiJaeCursorDragInterface, public ILS2DPuzzleControllerInterface, public ILSStopKeyInputInterface
{
	GENERATED_BODY()

public:
	ALSPlayerController();
	//수정 게터함수
	FORCEINLINE TObjectPtr<class ULSHUDWidget> GetLSHUDWidget() const { return LSHUDWidget; }
	FORCEINLINE TObjectPtr<class ULSHpWidget> GetLSHpWidget() const { return LSHpWidget; }

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	//void SelectNextSlot();

	//UFUNCTION(BlueprintCallable, Category = "HUD")
	//void SelectPreviousSlot();


protected:
	virtual void PostInitializeComponents() override;
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
	TObjectPtr<class ULSHUDWidget> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSHUDWidget> LSHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSHUDWidget> LSHUDWidget;


// Hp widghet Section
protected:
	UPROPERTY()
	TObjectPtr<class ULSHpWidget> HpWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSHpWidget> LSHpWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSHpWidget> LSHpWidget;


//Blood
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBloodWidget> BloodWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBloodWidget> BloodWidget;

public:

	UFUNCTION()
	void ShowBloodWidget();

	/*UFUNCTION(Server, Unreliable)
	void ServerShowBloodWidget();

	UFUNCTION(Client, Unreliable)
	void ClientShowBloodWidget();*/

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastShowBloodWidget();


	UFUNCTION()
	void RemoveBloodWidget();


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


// Script Section
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULSScriptWidget> ScriptWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULSScriptWidget> ScriptWidget;

public:
	UFUNCTION(BlueprintCallable, Category = "Script Widget")
	virtual 	void UpdateScriptWidget(const FString& ScriptText) override;



// MiniMap Section
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UMiniMapWidget> MiniMapWidgetClass;
	
	UPROPERTY()
	TObjectPtr<class UMiniMapWidget> MiniMapWidget;

	void CreateMinimapWidget();
	void RemoveMinimapWidget();
	UFUNCTION(Client, Unreliable)
	void ClientMinimapWidget();
	bool bIsMinimap;

// Menu Section
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UMenuWidget> MenuWidgetClass;

	UPROPERTY()
	TObjectPtr<class UMenuWidget> MenuWidget;

public:
	void OpenMenu();

	void MenuToLevel(const FString& Option);

	//클라->서버
	UFUNCTION(Server, Reliable)
	void ServerMenuToLevel(const FString& Option);
	UFUNCTION(Client, Reliable)
	void ClientSetMenuInputMode(bool bMenuVisible);



//Chat section
public:
	virtual void SetupInputComponent() override;

	
	void OpenChat();
	void CloseChat();

	//클라->서버
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& Text);

	//서버_> 클라
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FString& Sender, const FString& Text);

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI|Chat")
	TSubclassOf<class ULSChatWidget> ChatWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULSChatWidget> ChatWidget = nullptr;

	float LastChatTimeServer = -1000.f;


	void SetUIOnlyInput();
	void SetGameOnlyInput();



// Voice Section
public:
	void StartTalking();
	void StopTalking();


// Quest Widget
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class ULSQuestWidget> QuestWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class ULSQuestWidget> QuestWidget;

public:
	void CallQuestClear();
	void CallQuestStart(FLSQuestData InQuestData);
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
	void Start2DPuzzle(const FName& InWidgetName, const FVector2D& InGoalPos);
	void End2DPuzzle();
	void Update2DPuzzleTimer(float Timer);
	virtual void OnExit2DPuzzle(uint8 InIsExitTogether) override;
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


// Input Section
public:
	virtual void StopKeyInput() override;


// Sequence Section
public:
	UFUNCTION(BlueprintCallable)
	void StartSequence(bool InIsMapStart, bool InNeedQuestComplete, UFileMediaSource* InVideoSource, class USoundBase* InSoundSource);
	UFUNCTION(BlueprintCallable)
	void EndSequence(bool bIsMapStart, bool bisNeedQuestComplete);

	void EndWaitClient();

protected:
	UPROPERTY()
	TSubclassOf<class UUserWidget> MediaPlayerWidgetClass;

	UPROPERTY()
	TSubclassOf<class UUserWidget> WaitClientWidgetClass;

	UPROPERTY()
	TObjectPtr<class UUserWidget> WaitClientWidget;

//BGM Section
public:
	UPROPERTY(BlueprintAssignable)
	FOnBGMStartDelegate OnBGMStart;

	UPROPERTY(BlueprintAssignable)
	FOnBGMStopDelegate OnBGMStop;

protected:
	void StartBGM();
	void StopBGM();


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
	void MulticastRPCStart2DPuzzle(const FName& InWidgetName, const FVector2D& InGoalPos);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCEnd2DPuzzle();

	UFUNCTION(Server, Unreliable)
	void ServerRPCOnExit2DPuzzle();

	UFUNCTION(Server, Unreliable)
	void ServerRPCOnClear2DPuzzle();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCUpdate2DPuzzleTimer(float Timer);

	UFUNCTION(Server, Unreliable)
	void ServerRPCStartGame(bool bisNeedQuestComplete);

	UFUNCTION(Server, Unreliable)
	void ServerRPCStopMovement();

	UFUNCTION(Client, Unreliable)
	void ClientRPCStopKeyInput();

	UFUNCTION(Client, Unreliable)
	void ClientRPCCallQuestClear();

	UFUNCTION(Client, Unreliable)
	void ClientRPCCallQuestStart(FLSQuestData InQuestData);

	UFUNCTION(Client, Unreliable)
	void ClientRPCStartSequence(bool InIsMapStart, bool InNeedQuestComplete, UFileMediaSource* InVideoSource, class USoundBase* InSoundSource);
};