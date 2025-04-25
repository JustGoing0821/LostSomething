// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage/LSTrainGate.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALSTrainGate::ALSTrainGate()
{
	// Stage Section
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Level/Stage/LSStageMesh.LSStageMesh'"));
	if (StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	static FName GateNames[] = { TEXT("Gate1"), TEXT("Gate2"), TEXT("Gate3"), TEXT("Gate4"), TEXT("Gate5"), TEXT("Gate6") };

	for (FName GateName : GateNames)
	{

		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateName);
		Gate->SetStaticMesh(GateMeshRef.Object);
		Gate->SetupAttachment(Stage, GateName);
		Gate->SetWorldScale3D(FVector(1.0f, 0.2f, 1.0f));
		Gate->SetRelativeLocation(FVector(-50.0f, -10.0f, 0.0f));
		Gate->SetCollisionProfileName(TEXT("NoCollision"));
		Gates.Add(GateName, Gate);

		FName TriggerName = *GateName.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetBoxExtent(FVector(50.0f, 10.0f, 50.0f));
		GateTrigger->SetupAttachment(Stage, GateName);
		GateTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		GateTrigger->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
		GateTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrainGate::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GateName);
		GateTriggers.Add(GateTrigger);
	}

	CurrentOpenGate = FMath::TruncToInt(FMath::FRandRange(0.0f, 6.0f));
	bReplicates = true;
}

void ALSTrainGate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrainGate, CurrentOpenGate);
}

// Called when the game starts or when spawned
void ALSTrainGate::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				PostOpenGate = CurrentOpenGate;
				GateClose(PostOpenGate);
				CurrentOpenGate = FMath::TruncToInt(FMath::FRandRange(0.0f, 6.0f));
				GateOpen(CurrentOpenGate);
			}
		), 5.0f, true, 0.0f);
	}
}

void ALSTrainGate::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LS_LOG(LogLS, Log, TEXT("Called."));
}

void ALSTrainGate::GateOpen(int32 InGateNum)
{
	static FName GateNames[] = { TEXT("Gate1"), TEXT("Gate2"), TEXT("Gate3"), TEXT("Gate4"), TEXT("Gate5"), TEXT("Gate6") };
	Gates[GateNames[InGateNum]]->SetVisibility(false);
	GateTriggers[InGateNum]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSTrainGate::GateClose(int32 InGateNum)
{
	static FName GateNames[] = { TEXT("Gate1"), TEXT("Gate2"), TEXT("Gate3"), TEXT("Gate4"), TEXT("Gate5"), TEXT("Gate6") };
	Gates[GateNames[InGateNum]]->SetVisibility(true);
	GateTriggers[InGateNum]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSTrainGate::OnRep_CurrentOpenGate()
{
	LS_LOG(LogLS, Log, TEXT("Called : %d"), CurrentOpenGate);
	GateClose(PostOpenGate);
	PostOpenGate = CurrentOpenGate;
	GateOpen(CurrentOpenGate);
}

