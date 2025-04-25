// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/LevelTransferVolume.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LevelTest/Player/LTPlayerCharacter.h"
#include "LevelTest/Player/LTPlayerCharacterIJae.h"
#include "LevelTest/Player/LTPlayerCharacterSiJae.h"

// Sets default values
ALevelTransferVolume::ALevelTransferVolume()
{
	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));
	RootComponent = TransferVolume;
	TransferVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

// Called when the game starts or when spawned
void ALevelTransferVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelTransferVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	LS_LOG(LogLS, Error, TEXT("NotifyActorBeginOverlap Notify"));

	ALTPlayerCharacter* PlayerCharacter = Cast<ALTPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UWorld* World = GetWorld();
		if (!World) return;

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ACharacter::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			ACharacter* Character = Cast<ACharacter>(Actor);
			if (Character)
			{
				Character->SetActorHiddenInGame(true);
			}
		}

		//PlayerCharacter->SetActorHiddenInGame(true);

		if (OtherActor->HasAuthority())
		{
			GetWorld()->ServerTravel(TEXT("/Game/Level/TestMap/LTMovedMap"));
		}
		LS_LOG(LogLS, Log, TEXT("Map Moved"));
	}
	else
	{
		LS_LOG(LogLS, Log, TEXT("NoPlayerCharacter"));
	}
}

