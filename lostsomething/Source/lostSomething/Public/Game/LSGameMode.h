// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSGameMode : public AGameModeBase, public ILSQuestInterface
{
	GENERATED_BODY()
	
public:
	ALSGameMode();

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

// Quest Section
public:
	void QuestStart() override;
	void QuestComplete() override;

	FORCEINLINE class ALSQuestManager* GetQuestManager() { return QuestManager; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ALSQuestManager> QuestManager;

//Test Login
protected:
	void TestLoginProcess(class APlayerController* ResultController);
};
