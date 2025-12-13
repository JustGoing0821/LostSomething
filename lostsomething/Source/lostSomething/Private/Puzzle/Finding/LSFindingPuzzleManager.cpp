// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Finding/LSFindingPuzzleManager.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Quest/LSQuestManager.h"
#include "Puzzle/Finding/LSFindingPuzzle.h"

// Sets default values
ALSFindingPuzzleManager::ALSFindingPuzzleManager()
{
	bReplicates = true;
	PuzzleActivateEnum = ELSInteractionEnum::Quest11;
	CurrentQuestEnum = ELSInteractionEnum::Quest0;
}

// Called when the game starts or when spawned
void ALSFindingPuzzleManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSFindingPuzzleManager::PuzzleStart()
{
	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSFindingPuzzle::StaticClass(), FoundActors);


	int32 CorrectIndex = FMath::RandRange(0, FoundActors.Num() - 1);
	//LS_LOG(LogLSls, Log, TEXT("CorrectIndex : %d"), CorrectIndex);


	for (int32 Index = 0 ; Index < FoundActors.Num() ; Index++)
	{
		ALSFindingPuzzle* FindingPuzzle = Cast<ALSFindingPuzzle>(FoundActors[Index]);

		if (FindingPuzzle !=nullptr)
		{
			if (Index == CorrectIndex)
			{
				FindingPuzzle->SetPuzzleAnswer(true);
				FindingPuzzle->OnPuzzleCheck.BindUObject(this, &ALSFindingPuzzleManager::PuzzleCheck);
			}
			else
			{
				FindingPuzzle->SetPuzzleAnswer(false);
				FindingPuzzle->OnPuzzleCheck.BindUObject(this, &ALSFindingPuzzleManager::PuzzleCheck);
			}
		}
	}
}

void ALSFindingPuzzleManager::PuzzleCheck(uint8 inIsPuzzleCorrect)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (inIsPuzzleCorrect)
	{
		QuestClear();
	}
	else
	{
		PuzzleStart();
	}
}

void ALSFindingPuzzleManager::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSFindingPuzzleManager::OnQuestChange);
		}
	}
}

void ALSFindingPuzzleManager::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	FString EnumString = StaticEnum<ELSInteractionEnum>()->GetNameByValue(static_cast<int64>(InQuestEnum)).ToString();
	LS_LOG(LogLSls, Log, TEXT("EnumValue : %s"), *EnumString);

	CurrentQuestEnum = InQuestEnum;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			if (CurrentQuestEnum == PuzzleActivateEnum)
			{
				PuzzleStart();
			}
		}
	), 1.0f, false);
}

void ALSFindingPuzzleManager::QuestClear()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->QuestComplete();
		}
	}
}

