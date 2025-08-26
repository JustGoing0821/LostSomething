// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LSLevelTransferVolume.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSStopKeyInputInterface.h"
#include "Quest/LSQuestManager.h"

// Sets default values
ALSLevelTransferVolume::ALSLevelTransferVolume()
{
	//Collision
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = TriggerBox;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALSLevelTransferVolume::OnTransferTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ALSLevelTransferVolume::OnTransferTriggerEndOverlap);

	bReplicates = true;
	PuzzleActivateEnum = ELSInteractionEnum::Quest0;
}

// Called when the game starts or when spawned
void ALSLevelTransferVolume::BeginPlay()
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
						//LS_LOG(LogLS, Log, TEXT("Owner Setted."));
						break;
					}
				}
			}
		), 1.0f, false, 2.0f);
	}

	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSLevelTransferVolume::OnTransferTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		if (OverlapCharacter)
		{
			ILSStopKeyInputInterface* LSCharacter = Cast<ILSStopKeyInputInterface>(OverlapCharacter->GetController());
			if (LSCharacter)
			{
				CurrentTriggerPlayers ++;
				//LS_LOG(LogLS, Log, TEXT("CurrentTriggerPlayers = %d"), CurrentTriggerPlayers);
				if (CurrentTriggerPlayers == 2 && !MoveTargetMap.IsNull())
				{
					MulticastRPCTriggerDeactivate();

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
		}
	}
}

void ALSLevelTransferVolume::OnTransferTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		if (OverlapCharacter)
		{
			ILSCharacterChoiceInterface* LSCharacter = Cast<ILSCharacterChoiceInterface>(OverlapCharacter->GetController());
			if (LSCharacter)
			{
				CurrentTriggerPlayers--;
				LS_LOG(LogLS, Log, TEXT("CurrentTriggerPlayers = %d"), CurrentTriggerPlayers);
			}
		}
	}
}

void ALSLevelTransferVolume::SetTriggerBoxExtent(FVector InBoxSize)
{
	TriggerBox->SetBoxExtent(InBoxSize);
}

void ALSLevelTransferVolume::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSLevelTransferVolume::OnQuestChange);
		}
	}
}

void ALSLevelTransferVolume::TriggerActivate()
{
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSLevelTransferVolume::TriggerDeactivate()
{
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSLevelTransferVolume::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCTriggerActivate();
	}
	else
	{
		MulticastRPCTriggerDeactivate();
	}
}

void ALSLevelTransferVolume::MulticastRPCTriggerActivate_Implementation()
{
	TriggerActivate();
}

void ALSLevelTransferVolume::MulticastRPCTriggerDeactivate_Implementation()
{
	TriggerDeactivate();
}

