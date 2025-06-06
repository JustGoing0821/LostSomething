// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Obstacle/BossObstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Interface/LSTakeDamageInterface.h>
#include "Engine/DamageEvents.h"

// Sets default values
ABossObstacle::ABossObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 콜리전 컴포넌트 초기화
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(32.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComp;

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABossObstacle::OnOverlapBegin);


	// 메시 컴포넌트 초기화
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(CollisionComp);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시 자체 충돌 제거
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_underbody_wheel_01.SM_underbody_wheel_01"));

	if (MeshAsset.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(MeshAsset.Object);
	}

}

// Called when the game starts or when spawned
void ABossObstacle::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(3.0f);

	// 시작 시 정면 방향을 기억 (Z는 고정)
	MoveDirection = GetActorForwardVector();
	
}

// Called every frame
void ABossObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector NewLocation = GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation);

}

void ABossObstacle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,	AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		FDamageEvent DamageEvent;
		ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OtherActor);
		HitResult->TakeDamage(AttackDamage, DamageEvent, GetInstigatorController(), this);

	}
}