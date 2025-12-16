// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/LSQuestClearHack.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"


// Sets default values
ALSQuestClearHack::ALSQuestClearHack()
{
	//Collision
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	CollisionBox->SetBoxExtent(FVector(30.0f, 30.0f, 30.0f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootComponent = CollisionBox;

	bReplicates = true;

}

// Called when the game starts or when spawned
void ALSQuestClearHack::BeginPlay()
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
		), 15.0f, false, 2.0f);
	}
}

void ALSQuestClearHack::InteractionProcess(APlayerController* InPlayerController)
{
	if (HasAuthority())
	{
		QuestClear();
	}
	else
	{
		ServerRPCQuestClear();
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
	}
}

void ALSQuestClearHack::QuestClear()
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

void ALSQuestClearHack::ServerRPCQuestClear_Implementation()
{
	LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("Begin"));
	QuestClear();
}


