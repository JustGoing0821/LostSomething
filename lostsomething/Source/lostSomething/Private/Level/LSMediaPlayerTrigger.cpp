// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LSMediaPlayerTrigger.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "Character/Players/LSPlayerController.h"
#include "Character/Players/LSPlayer.h"

// Sets default values
ALSMediaPlayerTrigger::ALSMediaPlayerTrigger()
{
	//Collision
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	RootComponent = TriggerBox;

	bIsMapStart = false;
	bisNeedQuestComplete = false;
}

// Called when the game starts or when spawned
void ALSMediaPlayerTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALSMediaPlayerTrigger::OnTriggerBeginOverlap);
}

void ALSMediaPlayerTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (!HasAuthority()) return;

	if (!VideoSource || !SoundSource)
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No MediaSource!"));
		return;
	}

	APawn* OverlapPawn = Cast<APawn>(OtherActor);
	if (OverlapPawn)
	{
		if (ALSPlayer* Player = Cast<ALSPlayer>(OverlapPawn))
		{
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
				{
					if (PC->IsLocalController())
					{
						PC->StartSequence(true, bIsMapStart, bisNeedQuestComplete, VideoSource, SoundSource);
					}
					else
					{
						PC->ClientRPCStartSequence(true, bIsMapStart, bisNeedQuestComplete, VideoSource, SoundSource);
					}
				}
			}
		}
	}
	
}

