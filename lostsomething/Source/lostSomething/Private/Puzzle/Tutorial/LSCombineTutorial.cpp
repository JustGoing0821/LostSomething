// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSCombineTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "Quest/LSQuestManager.h"
#include "Net/UnrealNetwork.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSCombineTutorialInterface.h"
#include "Interface/LSScriptWidgetInterface.h"

// Sets default values
ALSCombineTutorial::ALSCombineTutorial()
{
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSCombineTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSCombineTutorial::OnTutorialTriggerEndOverlap);

	bReplicates = true;
	PuzzleActivateEnum = ELSInteractionEnum::Quest4;
}

void ALSCombineTutorial::BeginPlay()
{
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSCombineTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));
	if (HasAuthority())
	{
		ILSCombineTutorialInterface* WheelchairPlayer = Cast<ILSCombineTutorialInterface>(OtherActor);
		if (WheelchairPlayer)
		{
			if (WheelchairPlayer->isCombining())
			{
				QuestClear();
			}
			else
			{
				LS_LOG(LogLS, Warning, TEXT("Being Not Pushed."));
				ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
				ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(OverlapCharacter->GetController());
				FString Script = "Being Not Pushed.";
				ScriptController->UpdateScriptWidget(Script);
			}
		}
	}

}

void ALSCombineTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ALSCombineTutorial::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSCombineTutorial::OnQuestChange);
		}
	}
}

void ALSCombineTutorial::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
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

void ALSCombineTutorial::PuzzleActivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSCombineTutorial::PuzzleDeactivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSCombineTutorial::QuestClear()
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

void ALSCombineTutorial::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSCombineTutorial::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}

