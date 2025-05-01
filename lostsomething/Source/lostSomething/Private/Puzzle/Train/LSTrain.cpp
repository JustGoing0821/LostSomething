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

	// Train Section
	TrainTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainTrigger"));
	RootComponent = TrainTrigger;
	TrainTrigger->SetBoxExtent(FVector(50.0, 50.0f, 50.0f));
	TrainTrigger->SetCollisionProfileName(CPROFILE_LSPAWN);

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	//Replication
	bReplicates = true;
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
			CurrentTrainState = ETrainState::Waiting;
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
				{
					CurrentTrainState = ETrainState::Leaving;
					CurrentAlpha = 0.0f;
				}
			), 1.f, false, 3.0f);
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

	//DOREPLIFETIME(ALSTrain, ServerTrainMove);
	DOREPLIFETIME(ALSTrain, WaitLocation);
	DOREPLIFETIME(ALSTrain, LeaveLocation);
	DOREPLIFETIME(ALSTrain, CurrentTrainState);
}

//void ALSTrain::OnRep_ServerTrainMove()
//{
//	FVector CurrentLocation = ServerTrainMove;
//	SetActorLocation(CurrentLocation);
//}
