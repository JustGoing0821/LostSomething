// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/NormalTile.h"


// Sets default values
ANormalTile::ANormalTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
	}
	MeshComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.125f));

	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_FloorTile.MI_FloorTile'"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("SetVisibleSiJae() : Material"));
	}

}

// Called when the game starts or when spawned
void ANormalTile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ANormalTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

