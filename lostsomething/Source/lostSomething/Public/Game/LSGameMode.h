// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSLocationTransferInterface.h"
#include "Interaction/LSInteractionEnum.h"
#include "Interface/LSSijaeCursorPosInterface.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "Interface/LS2DPuzzleGameModeInterface.h"
#include "Interface/LSStartGameInterface.h"
#include "LSGameMode.generated.h"



/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSGameMode : public AGameModeBase, public ILSQuestInterface, public ILSLocationTransferInterface, public ILSSijaeCursorPosInterface, public ILSSiJaeCursorDragInterface, public ILS2DPuzzleGameModeInterface, public ILSStartGameInterface
{
	GENERATED_BODY()
	
//chat
public:
	//서버에서 받은 채팅 컨트롤러로 전달
	UFUNCTION()
	void BroadcastChatMessage(const FString& Sender, const FString& Text);

public:
	ALSGameMode();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartGame() override;

	FOnStartGameDelegate OnStartGame;
	FORCEINLINE virtual FOnStartGameDelegate& GetOnStartGameDelegate() override { return OnStartGame;	}

protected:
	virtual void BeginPlay() override;

protected:
	TSubclassOf<APawn> IJaePawnClass;
	TSubclassOf<APawn> SiJaePawnClass;
	TSubclassOf<APawn> VRPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character, Meta = (AllowPrivateAccess = "true"))
	bool bIsSiJaeServer;

	int32 CurrentPlayerCount;

private:
	bool IsVRPlayer(APlayerController* Controller) const;

// Quest Section
public:
	void QuestStart() override;
	void QuestComplete() override;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	FORCEINLINE class ALSQuestManager* GetQuestManager() { return QuestManager; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ALSQuestManager> QuestManager;

// Menu

public:
	void MenuOption(FString Option);


// Player Transfer
public:
	void TransferPlayerLocation(FVector InSijaeLocation, FVector InIjaeLocation) override;

// Test Login
protected:
	void TestLoginProcess(class APlayerController* ResultController);


// Script Section
protected:
	void BroadcastScript(const FString& InScript);


// 2D Puzzle Section
public:
	FORCEINLINE virtual const FVector2D& GetSiJaeCursorPos() override { return SiJaeCursorPos; }
	FORCEINLINE virtual void SetSiJaeCursorPos(const FVector2D& InSiJaeCursorPos) override { SiJaeCursorPos = InSiJaeCursorPos; }
	virtual void OnChangeSiJaeDragState(uint8 InIsSiJaeDragging) override;
	virtual void Start2DPuzzle(float Timer, const FName& InWidgetName, const FVector2D& InGoalPos) override;
	virtual void End2DPuzzle() override;
	virtual void OnClear2DPuzzle() override;

	FORCEINLINE virtual FOn2DPuzzleClearDelegate& Get2DPuzzleClearDelegate() override { return On2DPuzzleClear; }
	virtual void OnFailed2DPuzzle() override;
	FORCEINLINE virtual FOn2DPuzzleFailedDelegate& Get2DPuzzleFailedDelegate() override { return On2DPuzzleFailed; }

	FOn2DPuzzleClearDelegate On2DPuzzleClear;
	FOn2DPuzzleFailedDelegate On2DPuzzleFailed;

protected:
	FVector2D SiJaeCursorPos;
	uint8 bIsSiJaeDragging : 1;


// Puzzle Timer Section
public:

protected:
	FTimerHandle PuzzleTimerHandle;
	float CurrentPuzzleTime;

	void StartPuzzleTimer(float InPuzzleTimerCount);
	void SetPuzzleTimer();
	void EndPuzzleTimer();
};
