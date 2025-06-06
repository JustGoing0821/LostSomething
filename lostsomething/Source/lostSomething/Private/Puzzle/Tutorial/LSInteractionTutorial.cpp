// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSInteractionTutorial.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Puzzle/VendingMachine/LSVendingMachine.h"

// Sets default values
ALSInteractionTutorial::ALSInteractionTutorial()
{
}

// Called when the game starts or when spawned
void ALSInteractionTutorial::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* TargetActor = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSVendingMachine::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		for (AActor* FoundActor : FoundActors)
		{
			ALSVendingMachine* VendingMachine = Cast<ALSVendingMachine>(FoundActor);
			if (VendingMachine)
			{
				VendingMachine->OnVMPuzzleCheck.BindUObject(this, &ALSInteractionTutorial::PuzzleCheck);
			}
		}
		//LS_LOG(LogLS, Log, TEXT("VendingMachines Found : %d"), VendingMachines.Num());
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No ALSVendingMachine"));
	}
}

void ALSInteractionTutorial::PuzzleCheck(bool isPuzzleCorrect)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (HasAuthority() && isPuzzleCorrect)
	{
		QuestClear();
	}
	else if(!HasAuthority() && isPuzzleCorrect)
	{
		ServerRPCQuestClear();
	}
}

void ALSInteractionTutorial::QuestClear()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->QuestComplete();
		}
	}
}

void ALSInteractionTutorial::ServerRPCQuestClear_Implementation()
{
	QuestClear();
}

