// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSAttackTutorialManager.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Puzzle/Tutorial/LSAttackTutorial.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"

// Sets default values
ALSAttackTutorialManager::ALSAttackTutorialManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	CurrentClearCharacter = 0;
}

// Called when the game starts or when spawned
void ALSAttackTutorialManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
				{
					if (PlayerController && !PlayerController->IsLocalController())
					{
						SetOwner(PlayerController);
						break;
					}
				}
			}
		), 1.0f, false, 2.0f);
	}

	AActor* TargetActor = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSAttackTutorial::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		int32 CurrentMachine = 0;
		for (AActor* FoundActor : FoundActors)
		{
			ALSAttackTutorial* VendingMachine = Cast<ALSAttackTutorial>(FoundActor);
			if (VendingMachine)
			{
				VendingMachine->OnAttackTutorial.BindUObject(this, &ALSAttackTutorialManager::PuzzleCheckCall);
			}
		}
		//LS_LOG(LogLS, Log, TEXT("VendingMachines Found : %d"), VendingMachines.Num());
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No ALSVendingMachine"));
	}
}

void ALSAttackTutorialManager::PuzzleCheckCall()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		PuzzleCheck();
	}
	else
	{
		ServerRPCPuzzleCheck();
	}
}

void ALSAttackTutorialManager::PuzzleCheck()
{
	CurrentClearCharacter++;
	if (CurrentClearCharacter == 2)
	{
		QuestClear();
	}
	//LS_LOG(LogLS, Log, TEXT("CurrentClearCharacter : %d"), CurrentClearCharacter);
}

void ALSAttackTutorialManager::QuestClear()
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

void ALSAttackTutorialManager::ServerRPCPuzzleCheck_Implementation()
{
	PuzzleCheck();
}

