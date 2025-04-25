// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALSGameMode();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

protected:
	TSubclassOf<APawn> IJaePawnClass;
	TSubclassOf<APawn> SiJaePawnClass;

// Quest Section
public:
	void QuestStart();
	void QuestComplete();

	FORCEINLINE class ALSQuestManager* GetQuestManager() { return QuestManager; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ALSQuestManager> QuestManager;
};
