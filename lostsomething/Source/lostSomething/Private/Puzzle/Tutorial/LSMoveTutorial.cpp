// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSMoveTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "LevelTest/Game/LTGameMode.h"
#include "Game/LSGameMode.h"


// Sets default values
ALSMoveTutorial::ALSMoveTutorial()
{
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSMoveTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSMoveTutorial::OnTutorialTriggerEndOverlap);

}

// Called when the game starts or when spawned
void ALSMoveTutorial::BeginPlay()
{
	Super::BeginPlay();
	LS_LOG(LogLS, Log, TEXT("Begin"));
}

void ALSMoveTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LS_LOG(LogLS, Log, TEXT("Begin"));

	if (HasAuthority())
	{
		CorrectPeopleCount++;
		if (CorrectPeopleCount == 2)
		{
			QuestClear();
		}
	}
}

void ALSMoveTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		CorrectPeopleCount--;
	}
}

void ALSMoveTutorial::QuestClear()
{
	if (HasAuthority())
	{
		ALSGameMode* LSGameMode = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode());
		if (LSGameMode)
		{
			LSGameMode->QuestComplete();
		}
		else
		{
			ALTGameMode* LTGameMode = Cast<ALTGameMode>(GetWorld()->GetAuthGameMode());
			if (LTGameMode)
			{
				LTGameMode->QuestComplete();
			}
		}
	}
}
