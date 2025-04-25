// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest/LSQuestData.h"
#include "LSGameSingleton.generated.h"

#define LS_SINLOG(CatName, Verbosity, Format, ...) UE_LOG(CatName, Verbosity, TEXT("[%s] %s : %s"), TEXT("GAME_SINGLETON"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

/******Log format (for GameSingleton)*********

#include "lostSomething.h"

LS_SINLOG(LogLS, [Log,Warning,Error], ... ... );

**************************/

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	ULSGameSingleton();
	static ULSGameSingleton& Get();

//Quest Section
public:
	const TArray<FLSQuestData>& GetQuestData();

	UPROPERTY()
	int32 QuestMaxLevel;

private:
	TArray<FLSQuestData> QuestDataTable;
};
