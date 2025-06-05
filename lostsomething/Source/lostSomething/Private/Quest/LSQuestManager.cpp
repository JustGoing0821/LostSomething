// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/LSQuestManager.h"
#include "lostSomething.h"
#include "Game/LSGameSingleton.h"

// Sets default values
ALSQuestManager::ALSQuestManager()
{
	CurrentQuestIndex = 0;
}

// Called when the game starts or when spawned
void ALSQuestManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALSQuestManager::QuestStart()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	SetCurrentQuest(ULSGameSingleton::Get().GetQuestData()[CurrentQuestIndex]);
	LS_LOG(LogLS, Log, TEXT("QuestName : %s"), *CurrentQuestData.QuestName);
	//LS_LOG(LogLS, Log, TEXT("Description : %s"), *CurrentQuestData.Description);

	OnQuestStart.Broadcast(CurrentQuestData, CurrentQuestData.CurrentQuestEnum);
	OnInteractionChange.Broadcast(CurrentQuestData.CurrentQuestEnum);
}

void ALSQuestManager::QuestComplete()
{
	int32 QuestMaxLevel=0;
	ULSGameSingleton& GameSingleton = ULSGameSingleton::Get();
	QuestMaxLevel = GameSingleton.QuestMaxLevelIndex;

	if (CurrentQuestIndex < QuestMaxLevel)
	{
		OnQuestComplete.Broadcast();
		CurrentQuestIndex += 1;
		QuestStart();
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("Quest Max Level : %d"), QuestMaxLevel);
	}
}

