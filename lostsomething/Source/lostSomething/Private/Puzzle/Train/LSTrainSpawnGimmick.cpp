// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrainSpawnGimmick.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Puzzle/Train/LSTrain.h"
#include "LevelTest/Player/LTPlayerCharacter.h"

// Sets default values
ALSTrainSpawnGimmick::ALSTrainSpawnGimmick()
{
	// Stage Section
	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	RootComponent = StageTrigger;
	StageTrigger->SetBoxExtent(FVector(500.0, 500.0f, 300.0f));
	StageTrigger->SetCollisionProfileName(TEXT("LSTrigger"));
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnSpawnTriggerBeginOverlap);
	StageTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnSpawnTriggerEndOverlap);

	// Spawn Train
	TrainClass = ALSTrain::StaticClass();
	CurrentState = ETrainSpawnState::Despawned;
}

// Called when the game starts or when spawned
void ALSTrainSpawnGimmick::BeginPlay()
{
	Super::BeginPlay();
}

void ALSTrainSpawnGimmick::OnSpawnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALTPlayerCharacter* OverlapBeginActor = Cast<ALTPlayerCharacter>(OtherActor);
	if (OverlapBeginActor)
	{
		FString EnumString = StaticEnum<ETrainSpawnState>()->GetNameByValue(static_cast<int64>(CurrentState)).ToString();
		switch (CurrentState)
		{
		case ETrainSpawnState::Spawned:
			LS_LOG(LogLS, Log, TEXT("CurrentState : %s"), *EnumString);
			CurrentState = ETrainSpawnState::Waited;
			break;

		case ETrainSpawnState::Waited:
			LS_LOG(LogLS, Log, TEXT("CurrentState : %s"), *EnumString);
			CurrentState = ETrainSpawnState::Despawned;
			break;

		case ETrainSpawnState::Despawned:
			LS_LOG(LogLS, Log, TEXT("CurrentState : %s"), *EnumString);
			CurrentState = ETrainSpawnState::Spawned;
			if (HasAuthority())
			{
				float DelayTime = FMath::FRandRange(2.f, 6.f);
				LS_LOG(LogLS, Log, TEXT("DelayTime : %f"), DelayTime);
				FTimerHandle Handle;
				GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
					{
						FVector NewLocation = FVector(-90.0f, -900.0f, 50.0f);

						if (TrainClass)
						{
							AActor* OpponentTrain = GetWorld()->SpawnActor(TrainClass, &NewLocation, &FRotator::ZeroRotator);
						}
					}
				), 1.f, false, DelayTime);
			}
			break;

		default:
			LS_LOG(LogLS, Log, TEXT("%s"), TEXT("No CurrentState"));
			break;
		}
	}
}

void ALSTrainSpawnGimmick::OnSpawnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALSTrain* OverlapEndActor = Cast<ALSTrain>(OtherActor);
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

