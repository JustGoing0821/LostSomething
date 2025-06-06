// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/LSTrainStageGimmick.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Stage/LSTrainStage.h"

// Sets default values
ALSTrainStageGimmick::ALSTrainStageGimmick()
{
	// Stage Section
	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	RootComponent = StageTrigger;
	StageTrigger->SetBoxExtent(FVector(500.0, 500.0f, 300.0f));
	StageTrigger->SetCollisionProfileName(TEXT("LSTrigger"));
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainStageGimmick::OnStageTriggerBeginOverlap);
	StageTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSTrainStageGimmick::OnStageTriggerEndOverlap);

	// Spawn Train
	TrainClass = ALSTrainStage::StaticClass();

}

// Called when the game starts or when spawned
void ALSTrainStageGimmick::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALSTrainStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALSTrainStage* OverlapBeginActor = Cast<ALSTrainStage>(OtherActor);
	if (OverlapBeginActor)
	{
		return;
	}
	if (HasAuthority())
	{
		FVector NewLocation = FVector(-90.0f, -900.0f, 50.0f);

		if (TrainClass)
		{
			AActor* OpponentTrain = GetWorld()->SpawnActor(TrainClass, &NewLocation, &FRotator::ZeroRotator);
		}
	}
}

void ALSTrainStageGimmick::OnStageTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALSTrainStage* OverlapEndActor = Cast<ALSTrainStage>(OtherActor);
	if (OverlapEndActor)
	{
		OverlapEndActor->Destroy();
		LS_LOG(LogLS, Log, TEXT("Train Overlap Ended."));
	}
	else
	{
		LS_LOG(LogLS, Log, TEXT("Player Overlap Ended."));
	}
}

