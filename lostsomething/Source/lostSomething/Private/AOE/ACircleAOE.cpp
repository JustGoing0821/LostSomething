// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "AOE/ACircleAOE.h"

// Sets default values
ACircleAOE::ACircleAOE()
{

    bReplicates = true;

    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // 루트 컴포넌트 생성
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // 충돌 스피어 생성
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(RootComponent);
    CollisionSphere->SetSphereRadius(Radius);

    // 경고 메시 생성
    WarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningMesh"));
    WarningMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACircleAOE::BeginPlay()
{
    Super::BeginPlay();

    // 반지름 설정
    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }
}

// Called every frame
void ACircleAOE::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACircleAOE::StartAOE()
{
    UE_LOG(LogTemp, Warning, TEXT("AOE Started! Radius: %f, Duration: %f"), Radius, WarningDuration);

    // 경고 표시
    if (WarningMesh)
    {
        WarningMesh->SetVisibility(true);
    }

    // 디버그 원 그리기 (개발 중에만 사용)
#if WITH_EDITOR
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Red, false, WarningDuration);
#endif

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
    UE_LOG(LogTemp, Warning, TEXT("AOE Exploded!"));

    // 경고 메시 숨기기
    if (WarningMesh)
    {
        WarningMesh->SetVisibility(false);
    }

    // 폭발 이펙트 디버그
#if WITH_EDITOR
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Orange, false, 2.0f);
#endif

    // 범위 내 플레이어에게 데미지
    DealDamageToPlayersInRange();

    // 2초 후 삭제 (이펙트 확인용)
    FTimerHandle DestroyTimer;
    GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]()
        {
            Destroy();
        }, 2.0f, false);
}

void ACircleAOE::DealDamageToPlayersInRange()
{
    if (!CollisionSphere)
        return;

    // 충돌 스피어 내의 모든 액터 찾기
    TArray<AActor*> OverlappingActors;
    CollisionSphere->GetOverlappingActors(OverlappingActors);

    int32 DamagedCount = 0;

    for (AActor* Actor : OverlappingActors)
    {
        // 플레이어인지 확인
        if (APawn* Pawn = Cast<APawn>(Actor))
        {
            // 데미지 적용
            UGameplayStatics::ApplyDamage(
                Pawn,
                Damage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );

            DamagedCount++;
            UE_LOG(LogTemp, Warning, TEXT("Damaged Actor: %s for %f damage"), *Pawn->GetName(), Damage);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("AOE hit %d actors"), DamagedCount);
}