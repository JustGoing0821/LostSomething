// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSWheelchairTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSWheelchairTutorialInterface.h"

// Sets default values
ALSWheelchairTutorial::ALSWheelchairTutorial()
{
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSWheelchairTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSWheelchairTutorial::OnTutorialTriggerEndOverlap);
}

void ALSWheelchairTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LS_LOG(LogLS, Log, TEXT("Begin"));
	if (HasAuthority())
	{
		ILSWheelchairTutorialInterface* WheelchairPlayer = Cast<ILSWheelchairTutorialInterface>(OtherActor);
		if (WheelchairPlayer)
		{
			if (WheelchairPlayer->CheckWheelchairTutorial())
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

void ALSWheelchairTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ALSWheelchairTutorial::QuestClear()
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

