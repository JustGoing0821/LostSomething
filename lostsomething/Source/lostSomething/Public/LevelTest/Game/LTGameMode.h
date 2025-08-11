// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSSijaeCursorPosInterface.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "Interface/LS2DPuzzleInterface.h"
#include "LTGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTGameMode : public AGameModeBase, public ILSQuestInterface, public ILSSijaeCursorPosInterface, public ILSSiJaeCursorDragInterface, public ILS2DPuzzleInterface
{
	GENERATED_BODY()

public:
	ALTGameMode();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

protected:
	TSubclassOf<APawn> IJaePawnClass;
	TSubclassOf<APawn> SiJaePawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character, Meta = (AllowPrivateAccess = "true"))
	bool bIsSiJaeServer;

	int32 CurrentPlayerCount;

	void TestLoginProcess(class APlayerController* ResultController);


// Quest Section
public:
	void QuestStart() override;
	void QuestComplete() override;

	FORCEINLINE virtual class ALSQuestManager* GetQuestManager() override { return QuestManager; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ALSQuestManager> QuestManager;


// 2D Puzzle Section
public:
	FORCEINLINE virtual const FVector2D& GetSiJaeCursorPos() override { return SiJaeCursorPos; }
	FORCEINLINE virtual void SetSiJaeCursorPos(const FVector2D& InSiJaeCursorPos) override { SiJaeCursorPos = InSiJaeCursorPos; }
	virtual void OnChangeSiJaeDragState(uint8 InIsSiJaeDragging) override;
	virtual void Start2DPuzzle(FName InPuzzleName, uint8 InIsStartTogether, APlayerController* InPlayerController) override;
	virtual void End2DPuzzle(FName InPuzzleName, uint8 InIsEndTogether, APlayerController* InPlayerController) override;

protected:
	FVector2D SiJaeCursorPos;
	uint8 bIsSiJaeDragging : 1;
};
