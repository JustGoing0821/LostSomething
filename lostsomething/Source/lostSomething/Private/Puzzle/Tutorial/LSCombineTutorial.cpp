// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSCombineTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSCombineTutorialInterface.h"

// Sets default values
ALSCombineTutorial::ALSCombineTutorial()
{
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSCombineTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSCombineTutorial::OnTutorialTriggerEndOverlap);
}

void ALSCombineTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LS_LOG(LogLS, Log, TEXT("Begin"));
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
				LS_LOG(LogLS, Log, TEXT("Being Not Pushed."));
			}
		}
	}

}

void ALSCombineTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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

