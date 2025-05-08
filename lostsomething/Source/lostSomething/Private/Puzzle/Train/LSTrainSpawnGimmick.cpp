// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrainSpawnGimmick.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Puzzle/Train/LSTrain.h"
#include "LevelTest/Player/LTPlayerCharacter.h"
#include "Physics/LSCollisionProfile.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ALSTrainSpawnGimmick::ALSTrainSpawnGimmick()
{
	bReplicates = true;

	// Stage Section
	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(300, 10000, 200));
	StageTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	RootComponent = StageTrigger;
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnSpawnTriggerBeginOverlap);
	StageTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnSpawnTriggerEndOverlap);

	// Trigger Section
	static FName GateNames[] = { TEXT("Gate1") , TEXT("Gate2"), TEXT("Gate3"), TEXT("Gate4"), TEXT("Gate5"), TEXT("Gate6") };
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StepMeshRef(TEXT("/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> StepMaterialRef(TEXT("/Game/Level/InteractionActor/Materials/M_Blue.M_Blue"));
	FVector CreateLocation = FVector(-300, -1215, -50);
	for (FName GateName : GateNames)
	{
		FName WaitTriggerName = *GateName.ToString().Append(TEXT("WaitTrigger"));
		UBoxComponent* WaitTrigger = CreateDefaultSubobject<UBoxComponent>(WaitTriggerName);
		WaitTrigger->SetupAttachment(StageTrigger);
		WaitTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
		WaitTrigger->SetBoxExtent(FVector(100, 100, 100));
		WaitTrigger->SetRelativeLocation(CreateLocation);
		WaitTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnGateWaitTriggerBeginOverlap);
		WaitTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnGateWaitTriggerEndOverlap);
		WaitTrigger->ComponentTags.Add(GateName);
		WaitTriggers.Add(WaitTrigger);

		float CurrentGate = FCString::Atoi(*GateName.ToString().Right(1));
		CurrentOverlapTrigger.Add(CurrentGate, 0);

		CreateLocation += FVector(0, 400, 0);
	}

	// Spawn Train
	TrainClass = ALSTrain::StaticClass();
	CurrentState = ETrainSpawnState::Despawned;

	// Mesh
	//MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->SetupAttachment(RootComponent);
	//MeshComponent->SetWorldScale3D(FVector(4.0, 200.0f, 0.1f));
	//MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	//MeshComponent->SetVisibility(false);
	//MeshComponent->SetRelativeLocation(FVector(-100.0f,-10000.0f, -200.0f));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Level/Puzzle/Train/TrainSpawnMeshTest.TrainSpawnMeshTest"));
	//if (ItemMeshRef.Object)
	//{
	//	MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	//}

}

void ALSTrainSpawnGimmick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrainSpawnGimmick, CurrentState);
}

// Called when the game starts or when spawned
void ALSTrainSpawnGimmick::BeginPlay()
{
	Super::BeginPlay();

	/*
	FVector SpawnLocation = MeshComponent->GetSocketLocation("TrainSpawn") + FVector(0.0f, 0.0f, 200.0f);
	LS_LOG(LogLS, Log, TEXT("SpawnLocation : %f, %f, %f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
	FVector WaitLocation = MeshComponent->GetSocketLocation("TrainStop") + FVector(0.0f, 0.0f, 200.0f);
	LS_LOG(LogLS, Log, TEXT("WaitLocation : %f, %f, %f"), WaitLocation.X, WaitLocation.Y, WaitLocation.Z);
	FVector LeaveLocation = MeshComponent->GetSocketLocation("TrainLeave") + FVector(0.0f, 2000.0f, 200.0f);
	LS_LOG(LogLS, Log, TEXT("SpawnLocation : %f, %f, %f"), LeaveLocation.X, LeaveLocation.Y, LeaveLocation.Z);
	*/
	SpawnTrain();
}

void ALSTrainSpawnGimmick::OnSpawnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ALTPlayerCharacter* OverlapBeginActor = Cast<ALTPlayerCharacter>(OtherActor);
	//if (OverlapBeginActor)
	//{
	//	FString EnumString = StaticEnum<ETrainSpawnState>()->GetNameByValue(static_cast<int64>(CurrentState)).ToString();
	//	switch (CurrentState)
	//	{
	//	case ETrainSpawnState::Spawned:
	//		break;
	//	case ETrainSpawnState::Despawned:
	//		SpawnTrain();
	//		break;
	//	default:
	//		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("No CurrentState"));
	//		break;
	//	}
	//}
}

void ALSTrainSpawnGimmick::OnSpawnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALSTrain* OverlapEndActor = Cast<ALSTrain>(OtherActor);
	if (OverlapEndActor)
	{
		OverlapEndActor->Destroy();
		CurrentState = ETrainSpawnState::Despawned;
		SpawnTrain();
		//LS_LOG(LogLS, Log, TEXT("Train Overlap Ended."));
	}
}

void ALSTrainSpawnGimmick::OnGateWaitTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		check(OverlappedComponent->ComponentTags.Num() == 1);
		FName ComponentTag = OverlappedComponent->ComponentTags[0];
		int32 CurrentGate = FCString::Atoi(*ComponentTag.ToString().Right(1));
		CurrentOverlapTrigger[CurrentGate] += 1;
		//LS_LOG(LogLS, Log, TEXT("%d Gate : %d"), CurrentGate, CurrentOverlapTrigger[CurrentGate]);
	}

}

void ALSTrainSpawnGimmick::OnGateWaitTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		check(OverlappedComponent->ComponentTags.Num() == 1);
		FName ComponentTag = OverlappedComponent->ComponentTags[0];
		int32 CurrentGate = FCString::Atoi(*ComponentTag.ToString().Right(1));
		CurrentOverlapTrigger[CurrentGate] -= 1;
		//LS_LOG(LogLS, Log, TEXT("%d Gate : %d"), CurrentGate, CurrentOverlapTrigger[CurrentGate]);
	}
}

void ALSTrainSpawnGimmick::SpawnTrain()
{
	if (HasAuthority() && CurrentState == ETrainSpawnState::Despawned && TrainClass != nullptr)
	{
		CurrentState = ETrainSpawnState::Spawned;

		float DelayTime = FMath::FRandRange(2.f, 6.f);
		//LS_LOG(LogLS, Log, TEXT("DelayTime : %f"), DelayTime);

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				FVector SpawnLocation = FVector(700.0f, -9950.0f, 300.0f);
				FRotator SpawnRotation = FRotator(0.f, 90.f, 0.f);
				AActor* OpponentTrain = GetWorld()->SpawnActor(TrainClass, &SpawnLocation, &SpawnRotation);
			}
		), 1.f, false, DelayTime);
	}
}
