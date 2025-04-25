// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage/LSTrainStage.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALSTrainStage::ALSTrainStage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionProfileName(TEXT("LSCollision"));
	MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	//Replication
	bReplicates = true;
}

// Called every frame
void ALSTrainStage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector CurrentLocation = GetActorLocation();
		SetActorLocation(CurrentLocation + FVector(-5.0f, 0.0f, 0.0f));
		ServerTrainMove = CurrentLocation;
	}
}

void ALSTrainStage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrainStage, ServerTrainMove);
}

void ALSTrainStage::OnRep_ServerTrainMove()
{
	FVector CurrentLocation = ServerTrainMove;
	SetActorLocation(CurrentLocation);
}

