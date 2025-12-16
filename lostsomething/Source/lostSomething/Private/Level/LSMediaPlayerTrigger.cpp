// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LSMediaPlayerTrigger.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "Character/Players/LSPlayerController.h"
#include "Character/Players/LSPlayer.h"

// Sets default values
ALSMediaPlayerTrigger::ALSMediaPlayerTrigger()
{
	bReplicates = true;

	//Collision
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	RootComponent = TriggerBox;

	bIsMapStart = false;
	bisNeedQuestComplete = false;
	bisNeedLevelTransfer = false;
}

// Called when the game starts or when spawned
void ALSMediaPlayerTrigger::BeginPlay()
{
	Super::BeginPlay();
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALSMediaPlayerTrigger::OnTriggerBeginOverlap);

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
						//LS_LOG(LogLSls, Log, TEXT("Owner Setted."));
						break;
					}
				}
			}
		), 15.0f, false);
	}
}

void ALSMediaPlayerTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	if (!VideoSource || !SoundSource)
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No MediaSource!"));
		return;
	}

	if (!HasAuthority() && bisNeedLevelTransfer)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
			{
				//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Check ALSPlayerController"));
				if (PC->IsLocalController())
				{
					PC->OnBGMStart.AddDynamic(this, &ALSMediaPlayerTrigger::OnEndSequenceCalled);
					//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Binded"));
				}
			}
		}
	}

	if (!HasAuthority()) return;

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
						PC->StartSequence(bIsMapStart, bisNeedQuestComplete, VideoSource, SoundSource);
					}
					else
					{
						PC->ClientRPCStartSequence(bIsMapStart, bisNeedQuestComplete, VideoSource, SoundSource);
					}
				}
			}
		}
	}
	
}

void ALSMediaPlayerTrigger::OnEndSequenceCalled()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	StartLevelTransfer();
}

void ALSMediaPlayerTrigger::StartLevelTransfer_Implementation()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ILSStopKeyInputInterface* PC = Cast<ILSStopKeyInputInterface>(Iterator->Get()))
			{
				PC->StopKeyInput();
			}
		}

		FString MapPath = MoveTargetMap.GetLongPackageName();
		GetWorld()->ServerTravel(*MapPath);
	}
}

