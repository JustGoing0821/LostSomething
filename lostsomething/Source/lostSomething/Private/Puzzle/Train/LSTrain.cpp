// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrain.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"

ALSTrain::ALSTrain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Replication
	bReplicates = true;

	// Train Section
	TrainTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainTrigger"));
	RootComponent = TrainTrigger;
	TrainTrigger->SetBoxExtent(FVector(300.0f, 50.0f, 100.0f));
	TrainTrigger->SetCollisionProfileName(CPROFILE_LSPAWN);

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetRelativeScale3D(FVector(6.0f, 1.0f, 2.0f));
	MeshComponent->SetRelativeLocation(FVector(-300.0f, -50.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Level/Puzzle/Train/TrainMeshTest.TrainMeshTest"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	//Gates
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	static FName GateNames[] = { TEXT("Gate1") , TEXT("Gate2"), TEXT("Gate3"), TEXT("Gate4"), TEXT("Gate5"), TEXT("Gate6") };

	for (FName GateName : GateNames)
	{

		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateName);
		Gate->SetStaticMesh(GateMeshRef.Object);
		Gate->SetupAttachment(MeshComponent, GateName);
		Gate->SetWorldScale3D(FVector(0.1f, 0.1f, 0.5f));
		Gate->SetCollisionProfileName(TEXT("NoCollision"));
		Gates.Add(Gate);

		FName TriggerName = *GateName.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetupAttachment(MeshComponent, GateName);
		GateTrigger->SetBoxExtent(FVector(5.0f, 5.0f, 25.0f));
		GateTrigger->SetRelativeLocation(FVector(5.0f, 5.0f, 25.0f));
		GateTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
		GateTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrain::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GateName);
		GateTriggers.Add(GateTrigger);
	}
}

void ALSTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTrainState== ETrainState::Comming)
	{
		CurrentAlpha += DeltaTime * LerpSpeed;
		CurrentAlpha = FMath::Clamp(CurrentAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		ServerTrainMove = FMath::Lerp(CurrentLocation, WaitLocation, CurrentAlpha);
		SetActorLocation(ServerTrainMove);

		if (HasAuthority() && (CurrentAlpha==1.0f))
		{
			CurrentOpenGate = FMath::RandRange(0, 5);
			LS_LOG(LogLS, Log, TEXT("CurrentOpenGate : %d"), CurrentOpenGate+1);
			GateOpen();
			CurrentAlpha = 0.0f;
			CurrentTrainState = ETrainState::Waiting;
		}
	}
	else if (CurrentTrainState == ETrainState::Waiting)
	{
	}
	else if (CurrentTrainState == ETrainState::Leaving)
	{
		CurrentAlpha += DeltaTime * LerpSpeed;
		CurrentAlpha = FMath::Clamp(CurrentAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		ServerTrainMove = FMath::Lerp(CurrentLocation, LeaveLocation, CurrentAlpha);
		SetActorLocation(ServerTrainMove);
	}
}

void ALSTrain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrain, WaitLocation);
	DOREPLIFETIME(ALSTrain, LeaveLocation);
	DOREPLIFETIME(ALSTrain, CurrentTrainState);
	DOREPLIFETIME(ALSTrain, CurrentOpenGate);
}

void ALSTrain::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ALSTrain::GateOpen()
{
	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, FTimerDelegate::CreateLambda([&]
		{
			Gates[CurrentOpenGate]->SetVisibility(false);
			GateTriggers[CurrentOpenGate]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	), 1.f, false, 1.0f);

	FTimerHandle Handle2;
	GetWorld()->GetTimerManager().SetTimer(Handle2, FTimerDelegate::CreateLambda([&]
		{
			GateClose();
		}
	), 1.f, false, 4.0f);
}

void ALSTrain::GateClose()
{
	Gates[CurrentOpenGate]->SetVisibility(true);
	//GateTriggers[CurrentOpenGate]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				CurrentTrainState = ETrainState::Leaving;
			}
		), 1.f, false, 1.0f);
	}
}

void ALSTrain::OnRep_CurrentOpenGate()
{
	GateOpen();
}
