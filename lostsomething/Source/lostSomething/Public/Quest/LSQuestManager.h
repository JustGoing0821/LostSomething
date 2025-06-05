// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest/LSQuestData.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSQuestManager.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuestStartDelegate, FLSQuestData /*CurrentQuestData*/, ELSInteractionEnum /*CurrentQuestEnum*/);
DECLARE_MULTICAST_DELEGATE(FOnQuestCompleteDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionChangeDelegate, ELSInteractionEnum /*CurrentQuestEnum*/);


UCLASS()
class LOSTSOMETHING_API ALSQuestManager : public AActor
{
	GENERATED_BODY()
	
	public:
	// Sets default values for this actor's properties
	ALSQuestManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	FOnQuestStartDelegate OnQuestStart;
	FOnQuestCompleteDelegate OnQuestComplete;
	FOnInteractionChangeDelegate OnInteractionChange;

	FORCEINLINE FLSQuestData GetCurrentQuest() const { return CurrentQuestData; }
	FORCEINLINE void SetCurrentQuest(const FLSQuestData& InCurrentQuest) { CurrentQuestData = InCurrentQuest; }

	void QuestStart();
	void QuestComplete();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Quest, Meta = (AllowPrivateAccess = "true"))
	FLSQuestData CurrentQuestData;

};
