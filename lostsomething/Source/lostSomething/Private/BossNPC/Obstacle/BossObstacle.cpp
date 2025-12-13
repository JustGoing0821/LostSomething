// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Obstacle/BossObstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Interface/LSTakeDamageInterface.h>
#include "Engine/DamageEvents.h"
#include <Components/CapsuleComponent.h>

// Sets default values
ABossObstacle::ABossObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// 콜리전 컴포넌트 초기화
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
	//CollisionComp->InitSphereRadius(16.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComp;

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABossObstacle::OnOverlapBegin);

	// 메시 컴포넌트 초기화
	TArray<FString> MeshPaths = {
		TEXT("/Game/Asset/Map/ModSubwayStation/StaticMeshes/SM_ScreenStanding.SM_ScreenStanding"),
		
	}; 
	//TEXT("/ Game / Asset / Map / CitySubwayTrainModuler / Meshes / Props / SM_fireex.SM_fireex")
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(CollisionComp);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시 자체 충돌 제거

	int32 RandomIndex = FMath::RandRange(0, MeshPaths.Num() - 1);
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(*MeshPaths[RandomIndex]);
	if (MeshAsset.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(MeshAsset.Object);
	}

	if (MeshAsset.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(MeshAsset.Object);
		ObstacleMesh->SetRelativeScale3D(FVector(1.0f));
		ObstacleMesh->SetRelativeLocation(FVector(-5.0f, 40.0f, -107.0f)); // Z축으로 50만큼 올림
		ObstacleMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

}

void ABossObstacle::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(8.5f);

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
		if (OtherActor && OtherActor != this)
		{
			ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OtherActor);
			if (HitResult) // 꼭 체크!
			{
				FDamageEvent DamageEvent;
				HitResult->TakeDamage(AttackDamage, DamageEvent, GetInstigatorController(), this);
			}
		}
	}
}