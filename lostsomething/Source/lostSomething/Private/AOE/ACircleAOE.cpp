// Fill out your copyright notice in the Description page of Project Settings.

#include "AOE/ACircleAOE.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Character/Players/LSPlayer.h" 
#include "BossNPC/BossNPC.h"          
#include "Components/SphereComponent.h"   
#include "Engine/DamageEvents.h"  
#include "GameFramework/DamageType.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACircleAOE::ACircleAOE()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = true;

    // 루트 컴포넌트 생성
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // 충돌 스피어 생성
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(RootComponent);
    CollisionSphere->SetSphereRadius(Radius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 경고 메시 생성
    WarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningMesh"));
    WarningMesh->SetupAttachment(RootComponent);
    WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMesh.Succeeded())
    {
        WarningMesh->SetStaticMesh(SphereMesh.Object);
        UE_LOG(LogTemp, Warning, TEXT("Sphere mesh loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Sphere mesh"));
    }

    WarningMesh->SetRelativeLocation(FVector(0, 0, -45.0f)); // 바닥에 붙이기
    WarningMesh->SetRelativeRotation(FRotator(0, 0, 0));

    float ScaleXY = Radius / 50.0f;
    float ScaleZ = 0.05f;
    WarningMesh->SetRelativeScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));
}

// Called when the game starts or when spawned
void ACircleAOE::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }

    if (WarningMesh)
    {
        float ScaleXY = Radius / 50.0f;
        float ScaleZ = 0.1f; 
        WarningMesh->SetRelativeScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));

        WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WarningMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
        WarningMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        WarningMesh->SetCanEverAffectNavigation(false); 

        UE_LOG(LogTemp, Warning, TEXT("WarningMesh configured: Scale(%f, %f, %f)"),
            ScaleXY, ScaleXY, ScaleZ);
    }

    // Dynamic Material Instance 생성
    if (WarningMaterial && WarningMesh)
    {
        DynamicMaterial = WarningMesh->CreateDynamicMaterialInstance(0, WarningMaterial);
        if (DynamicMaterial)
        {
            UE_LOG(LogTemp, Warning, TEXT("DynamicMaterial created successfully!"));
            DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor(1.0f, 0.5f, 0.0f, 0.8f));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WarningMaterial or WarningMesh is null!"));
    }
}

// Called every frame
void ACircleAOE::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsWarningPhase)
    {
        ElapsedTime += DeltaTime;
        float Alpha = ElapsedTime / WarningDuration;
        UpdateColorAnimation(Alpha);
    }
}

void ACircleAOE::StartAOE()
{
    bIsWarningPhase = true;
    ElapsedTime = 0.0f;

    // 경고 표시
    if (WarningMesh)
    {
        WarningMesh->SetVisibility(true);
    }

    /*
#if WITH_EDITOR
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Red, false, WarningDuration);
#endif
    */

    // 경고 시간 후 폭발
    GetWorld()->GetTimerManager().SetTimer(
        WarningTimerHandle,
        this,
        &ACircleAOE::Explode,
        WarningDuration,
        false
    );
}

void ACircleAOE::Explode()
{
    bIsWarningPhase = false;

    // 경고 메시 숨기기
    if (WarningMesh)
    {
        WarningMesh->SetVisibility(false);
    }

    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    /*
#if WITH_EDITOR
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Orange, false, 2.0f);
#endif
    */

    // 범위 내 플레이어에게 데미지
    DealDamageToPlayersInRange();

    Destroy();
}

void ACircleAOE::UpdateColorAnimation(float Alpha)
{
    if (DynamicMaterial)
    {
        FLinearColor BaseColor = FMath::Lerp(
            FLinearColor(1.0f, 0.6f, 0.0f, 0.8f),
            FLinearColor(1.0f, 0.0f, 0.0f, 0.9f),
            Alpha
        );

        float BlinkSpeed = FMath::Lerp(5.0f, 15.0f, Alpha);
        float BlinkValue = (FMath::Sin(ElapsedTime * BlinkSpeed) + 1.0f) * 0.5f;

        FLinearColor FinalColor = BaseColor;
        FinalColor.A *= (0.5f + BlinkValue * 0.5f);

        DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FinalColor);
    }
}

void ACircleAOE::DealDamageToPlayersInRange()
{
    if (!CollisionSphere)
        return;

    TArray<AActor*> OverlappingActors;
    CollisionSphere->GetOverlappingActors(OverlappingActors);

    int32 DamagedCount = 0;
    for (AActor* Actor : OverlappingActors)
    {
        if (Actor->IsA<ABossNPC>())
        {
            continue;
        }

        if (ALSPlayer* Player = Cast<ALSPlayer>(Actor))
        {
            FDamageEvent DamageEvent(UDamageType::StaticClass());
            Player->TakeDamage(Damage, DamageEvent, nullptr, this);
            DamagedCount++;
        }
    }
}