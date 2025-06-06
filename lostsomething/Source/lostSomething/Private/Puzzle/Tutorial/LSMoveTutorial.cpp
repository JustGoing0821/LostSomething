// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSMoveTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "Interface/LSQuestInterface.h"
#include "Quest/LSQuestManager.h"
#include "Interaction/LSInteractionEnum.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Character/Players/LSCharacterChoice.h"

// Sets default values
ALSMoveTutorial::ALSMoveTutorial()
{
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSMoveTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSMoveTutorial::OnTutorialTriggerEndOverlap);

	PuzzleActivateEnum = ELSInteractionEnum::Quest1;
}

// Called when the game starts or when spawned
void ALSMoveTutorial::BeginPlay()
{
	Super::BeginPlay();
	//LS_LOG(LogLS, Log, TEXT("Begin"));
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSMoveTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));

	if (HasAuthority())
	{
		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		if (OverlapCharacter)
		{
			ILSCharacterChoiceInterface* CharacterChoice = Cast<ILSCharacterChoiceInterface>(OverlapCharacter->GetController());
			if (CharacterChoice)
			{
				CurrentTriggerPlayers++;
				LS_LOG(LogLS, Log, TEXT("CurrentTriggerPlayers = %d"), CurrentTriggerPlayers);
				if (CurrentTriggerPlayers == 2)
				{
					QuestClear();
					return;
				}

				ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(OverlapCharacter->GetController());
				FString Script = "";
				if (CharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
				{
					Script = TEXT("Wait For IJae.");
				}
				else
				{
					Script = TEXT("Wait For SiJae.");
				}
				ScriptController->UpdateScriptWidget(Script);

			}
		}
	}
}

void ALSMoveTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		if (OverlapCharacter)
		{
			ILSScriptWidgetInterface* LSCharacter = Cast<ILSScriptWidgetInterface>(OverlapCharacter->GetController());
			if (LSCharacter)
			{
				CurrentTriggerPlayers--;
				LS_LOG(LogLS, Log, TEXT("CurrentTriggerPlayers = %d"), CurrentTriggerPlayers);
			}
		}
	}
}

void ALSMoveTutorial::BindQuestChange()
{
	if (HasAuthority())
	{
		//LS_LOG(LogLS, Log, TEXT("Begin"));
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSMoveTutorial::OnQuestChange);
		}
	}
}

void ALSMoveTutorial::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
	}
}

void ALSMoveTutorial::PuzzleActivate()
{
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSMoveTutorial::PuzzleDeactivate()
{
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSMoveTutorial::QuestClear()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->QuestComplete();
		}
	}
}

void ALSMoveTutorial::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSMoveTutorial::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}
