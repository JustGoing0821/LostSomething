// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrainSpawnGimmick.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Puzzle/Train/LSTrain.h"
#include "Puzzle/Train/LSTrainStep.h"
#include "LevelTest/Player/LTPlayerCharacter.h"
#include "Physics/LSCollisionProfile.h"
#include "Net/UnrealNetwork.h"
#include "LevelTest/Player/LTPlayerController.h"


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
	FVector CreateGateLocation = FVector(-300, -1215, -50);
	FVector CreateStepLocation = FVector(470, -1215, 210);
	for (FName GateName : GateNames)
	{
		FName WaitTriggerName = *GateName.ToString().Append(TEXT("WaitTrigger"));
		UBoxComponent* WaitTrigger = CreateDefaultSubobject<UBoxComponent>(WaitTriggerName);
		WaitTrigger->SetupAttachment(StageTrigger);
		WaitTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
		WaitTrigger->SetBoxExtent(FVector(100, 100, 100));
		WaitTrigger->SetRelativeLocation(CreateGateLocation);
		WaitTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnGateWaitTriggerBeginOverlap);
		WaitTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSTrainSpawnGimmick::OnGateWaitTriggerEndOverlap);
		WaitTrigger->ComponentTags.Add(GateName);
		WaitTriggers.Add(WaitTrigger);

		float CurrentGate = FCString::Atoi(*GateName.ToString().Right(1));
		CurrentOverlapTrigger.Add(CurrentGate, 0);

		StepTriggerLocations.Add(CreateStepLocation);

		CreateGateLocation += FVector(0, 400, 0);
		CreateStepLocation += FVector(0, 400, 0);
	}

	// Spawn Train
	TrainClass = ALSTrain::StaticClass();
	CurrentState = ETrainSpawnState::Despawned;
	StepTriggerClass = ALSTrainStep::StaticClass();

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

	// Pannel Section
	PannelMesh = CreateDefaultSubobject<UStaticMeshComponent>("PannelMesh");
	PannelMesh->SetupAttachment(RootComponent);
	PannelMesh->SetRelativeLocationAndRotation(FVector(300, -415, 50), FRotator(0, 90, 0));
	PannelMesh->SetRelativeScale3D(FVector(2,2,2));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PannelMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/MetroPanel/SM_MetroPanel.SM_MetroPanel"));
	if (PannelMeshRef.Object)
	{
		PannelMesh->SetStaticMesh(PannelMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef0(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_0.M_MetroPanel_0"));
	PannelMaterials.Add(PannelMaterialRef0.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef1(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_1.M_MetroPanel_1"));
	PannelMaterials.Add(PannelMaterialRef1.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef2(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_2.M_MetroPanel_2"));
	PannelMaterials.Add(PannelMaterialRef2.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef3(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_3.M_MetroPanel_3"));
	PannelMaterials.Add(PannelMaterialRef3.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef4(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_4.M_MetroPanel_4"));
	PannelMaterials.Add(PannelMaterialRef4.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef5(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_5.M_MetroPanel_5"));
	PannelMaterials.Add(PannelMaterialRef5.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PannelMaterialRef6(TEXT("/Game/Level/Puzzle/Train/Materials/MetroPanelMaterial/Instances/M_MetroPanel_6.M_MetroPanel_6"));
	PannelMaterials.Add(PannelMaterialRef6.Object);

	if (PannelMeshRef.Object)
	{
		PannelMesh->SetMaterial(0, PannelMaterials[0]);
	}

	//Puzzle
	CorrectGate = -1;
	CorrectPeopleCount = 2;
}

void ALSTrainSpawnGimmick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrainSpawnGimmick, CurrentState);
	DOREPLIFETIME(ALSTrainSpawnGimmick, CorrectGate);
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

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			if (HasAuthority())
			{
				SpawnTrain();
			}
		}
	), 1.f, false, 3);
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
		OnTrainDespawned.ExecuteIfBound();
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

		CorrectGate = FMath::RandRange(1, 6);
		LS_LOG(LogLS, Log, TEXT("CorrectGate : %d"), CorrectGate);

		float DelayTime = 10;// FMath::FRandRange(2.f, 6.f);
		LS_LOG(LogLS, Log, TEXT("DelayTime : %f"), DelayTime);

		MulticastRPCSetPannelMonitor();

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				FVector SpawnLocation = FVector(700.0f, -9950.0f, 300.0f);
				FRotator SpawnRotation = FRotator(0.f, 90.f, 0.f);
				AActor* OpponentTrain = GetWorld()->SpawnActor(TrainClass, &SpawnLocation, &SpawnRotation);
				ALSTrain* LSTrain = Cast<ALSTrain>(OpponentTrain);
				if (LSTrain)
				{
					LSTrain->SetCorrectGate(CorrectGate);
					LSTrain->OnTrainArrived.AddUObject(this, &ALSTrainSpawnGimmick::CheckPuzzleCorrect);
					LSTrain->DelegateBind(this);
				}
			}
		), 1.f, false, DelayTime);
	}
}

void ALSTrainSpawnGimmick::CheckPuzzleCorrect()
{
	if (CurrentOverlapTrigger[CorrectGate] == CorrectPeopleCount)
	{
		OnPuzzleCheck.Broadcast(true, CorrectGate);
		if (StepTriggerClass)
		{
			FVector SpawnLocation = StepTriggerLocations[CorrectGate-1];
			FRotator SpawnRotation = FRotator(0.f, 90.f, 0.f);
			AActor* OpponentStepTrigger = GetWorld()->SpawnActor(StepTriggerClass, &SpawnLocation, &SpawnRotation);
			OnTrainDespawned.BindLambda([OpponentStepTrigger]
				{
					OpponentStepTrigger->Destroy();
				});
			//LS_LOG(LogLS, Log, TEXT("StepTriggerClass Created."));
		}
	}
	else
	{
		OnPuzzleCheck.Broadcast(false, CorrectGate);
	}
}

void ALSTrainSpawnGimmick::SetPannelMonitor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	ALTPlayerController* LSController = Cast<ALTPlayerController>(PlayerController);
	if (LSController)
	{
		if (LSController->CharacterChoice == ECharacterChoice::IJae)
		{
			PannelMesh->SetMaterial(0, PannelMaterials[CorrectGate]);
		}
	}
}

void ALSTrainSpawnGimmick::OnRep_CorrectGate()
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
}

void ALSTrainSpawnGimmick::MulticastRPCSetPannelMonitor_Implementation()
{
	SetPannelMonitor();
}
