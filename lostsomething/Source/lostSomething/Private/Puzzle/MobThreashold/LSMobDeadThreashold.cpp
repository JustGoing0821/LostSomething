// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/MobThreashold/LSMobDeadThreashold.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Quest/LSQuestManager.h"
#include "NPC/TestNPC.h"

ALSMobDeadThreashold::ALSMobDeadThreashold()
{
	TargetKilledMobCount = 5;
	CurrentKilledMobCount = 0;
	MobCleanEnum = ELSInteractionEnum::Quest0;
}

void ALSMobDeadThreashold::CountDeadMobs()
{
	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		CurrentKilledMobCount++;
		if (CurrentKilledMobCount >= TargetKilledMobCount)
		{
			MulticastRPCPuzzleDeactivate();
			TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ALSMobDeadThreashold::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	Super::OnQuestChange(InQuestData, InQuestEnum);

	if (MobCleanEnum == ELSInteractionEnum::Quest0) return;

	if (InQuestEnum >= MobCleanEnum)
	{
		CleanAllMobs();
	}
}

void ALSMobDeadThreashold::CleanAllMobs()
{
	if (HasAuthority())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestNPC::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			Actor->Destroy();
		}
	}
}
