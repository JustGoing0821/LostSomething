// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSGameSingleton.h"
#include "lostSomething.h"

ULSGameSingleton::ULSGameSingleton()
{
	//Quest Data
	static ConstructorHelpers::FObjectFinder<UDataTable> QuestDataTableRef(TEXT("/Game/Stage/QuestData/LSQuestDataTable.LSQuestDataTable"));
	if (nullptr != QuestDataTableRef.Object)
	{
		const UDataTable* QuestTempTable = QuestDataTableRef.Object;
		check(QuestTempTable->GetRowMap().Num() > 0);

		TArray<uint8*> QuestTempValueArray;
		QuestTempTable->GetRowMap().GenerateValueArray(QuestTempValueArray);
		Algo::Transform(QuestTempValueArray, QuestDataTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FLSQuestData*>(Value);
			}
		);

		QuestMaxLevelIndex = QuestDataTable.Num()-1;
		//LS_SINLOG(LogLS, Log, TEXT("Quest Data Table Loaded."));
	}
}

ULSGameSingleton& ULSGameSingleton::Get()
{
	ULSGameSingleton* Singleton = CastChecked<ULSGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		//LS_SINLOG(LogLS, Log, TEXT("Singleton Getted."));
		return *Singleton;
	}
	else
	{
		LS_SINLOG(LogLS, Error, TEXT("Invalid Game Singleton."));
		return *NewObject<ULSGameSingleton>();
	}
}

const TArray<FLSQuestData>& ULSGameSingleton::GetQuestData()
{
	//LS_SINLOG(LogLS, Log, TEXT("QuestData Getted."));
	return QuestDataTable;
}
