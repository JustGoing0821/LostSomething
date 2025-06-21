// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/LSQuestManager.h"
#include "lostSomething.h"
#include "Game/LSGameSingleton.h"
#include "Game/LSGameInstance.h"

// Sets default values
ALSQuestManager::ALSQuestManager()
{
}

// Called when the game starts or when spawned
void ALSQuestManager::BeginPlay()
{
	Super::BeginPlay();
}

void ALSQuestManager::QuestStart()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	int32 CurrentQuestIndex = 0;
	ULSGameInstance* GameInstance = Cast<ULSGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		CurrentQuestIndex = GameInstance->GetCurrentQuestIndex();
	}

	SetCurrentQuest(ULSGameSingleton::Get().GetQuestData()[CurrentQuestIndex]);
	//LS_LOG(LogLS, Log, TEXT("QuestName : %s"), *CurrentQuestData.QuestName);
	//LS_LOG(LogLS, Log, TEXT("Description : %s"), *CurrentQuestData.Description);

	OnQuestStart.Broadcast(CurrentQuestData, CurrentQuestData.CurrentQuestEnum);
	OnInteractionChange.Broadcast(CurrentQuestData.CurrentQuestEnum);
}

void ALSQuestManager::QuestComplete()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	int32 QuestMaxLevel=0;
	ULSGameSingleton& GameSingleton = ULSGameSingleton::Get();
	QuestMaxLevel = GameSingleton.QuestMaxLevelIndex;

	int32 CurrentQuestIndex = 0;
	ULSGameInstance* GameInstance = Cast<ULSGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		CurrentQuestIndex = GameInstance->GetCurrentQuestIndex();
		if (CurrentQuestIndex < QuestMaxLevel)
		{
			OnQuestComplete.Broadcast();
			CurrentQuestIndex += 1;
			GameInstance->SetCurrentQuestIndex(CurrentQuestIndex);
			QuestStart();
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("Quest Max Level : %d"), QuestMaxLevel);
		}
	}
}

