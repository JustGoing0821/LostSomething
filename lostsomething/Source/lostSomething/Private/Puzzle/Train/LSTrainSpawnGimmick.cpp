// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrainSpawnGimmick.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Puzzle/Train/LSTrain.h"
#include "LevelTest/Player/LTPlayerCharacter.h"
#include "Physics/LSCollisionProfile.h"

// Sets default values
ALSTrainSpawnGimmick::ALSTrainSpawnGimmick()
{
	// Stage Section
	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	RootComponent = StageTrigger;
	StageTrigger->SetBoxExtent(FVector(300, 10000, 200));
	StageTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnSpawnTriggerBeginOverlap);
	StageTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnSpawnTriggerEndOverlap);

	// Spawn Train
	TrainClass = ALSTrain::StaticClass();
	CurrentState = ETrainSpawnState::Despawned;

	// Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetWorldScale3D(FVector(6.0, 200.0f, 1.0f));
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeLocation(FVector(-300.0f,-10000.0f, -200.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Level/Puzzle/Train/TrainSpawnMeshTest.TrainSpawnMeshTest"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

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
			break;
		case ETrainSpawnState::Despawned:
			CurrentState = ETrainSpawnState::Spawned;
			if (HasAuthority())
			{
				float DelayTime = FMath::FRandRange(2.f, 6.f);
				LS_LOG(LogLS, Log, TEXT("DelayTime : %f"), DelayTime);
				FTimerHandle Handle;
				GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
					{
						FVector SpawnLocation = MeshComponent->GetSocketLocation("TrainSpawn") + FVector(0.0f, 0.0f, 100.0f);
						FRotator SpawnRotation = FRotator(0.f, 90.f, 0.f);
						//LS_LOG(LogLS, Log, TEXT("SpawnLocation : %f, %f, %f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);

						if (TrainClass)
						{
							AActor* OpponentTrain = GetWorld()->SpawnActor(TrainClass, &SpawnLocation, &SpawnRotation);
							ALSTrain* LSTrain = Cast<ALSTrain>(OpponentTrain);
							if (LSTrain)
							{
								LSTrain->WaitLocation = MeshComponent->GetSocketLocation("TrainStop") + FVector(0.0f, 0.0f, 100.0f);
								//LS_LOG(LogLS, Log, TEXT("WaitLocation : %f, %f, %f"), LSTrain->WaitLocation.X, LSTrain->WaitLocation.Y, LSTrain->WaitLocation.Z);
								LSTrain->LeaveLocation = MeshComponent->GetSocketLocation("TrainLeave") + FVector(0.0f, 2000.0f, 1000.0f);
								//LS_LOG(LogLS, Log, TEXT("SpawnLocation : %f, %f, %f"), LSTrain->LeaveLocation.X, LSTrain->LeaveLocation.Y, LSTrain->LeaveLocation.Z);
							}
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
		CurrentState = ETrainSpawnState::Despawned;
		LS_LOG(LogLS, Log, TEXT("Train Overlap Ended."));
	}
}

