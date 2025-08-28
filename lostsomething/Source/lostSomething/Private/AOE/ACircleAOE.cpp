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
#include "Net/UnrealNetwork.h"

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

    // Share AOE 기본 설정
    bShouldTrackPlayer = true;
    TrackingSpeed = 500.0f;
}

void ACircleAOE::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACircleAOE, ElapsedTime);
    DOREPLIFETIME(ACircleAOE, bIsWarningPhase);
    DOREPLIFETIME(ACircleAOE, TrackedPlayer);
}

// Called when the game starts or when spawned
void ACircleAOE::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }

    SetupWarningVisual();

    // Cone AOE인 경우 강제로 보이게 설정
    if (AOEType == EAOEType::Cone && WarningMesh)
    {
        WarningMesh->SetVisibility(true);
        WarningMesh->SetHiddenInGame(false);
        UE_LOG(LogTemp, Warning, TEXT("CONE AOE: Forced visibility ON in BeginPlay"));
    }
}

// ========================================
// AOE 타입별 설정 함수들
// ========================================

void ACircleAOE::SetupAsCircleAOE(float InRadius)
{
    AOEType = EAOEType::Circle;
    Radius = InRadius;
    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }
    SetupWarningVisual();
}

void ACircleAOE::SetupAsConeAOE(float InRange, float InAngle, FVector InDirection)
{
    AOEType = EAOEType::Cone;
    ConeRange = FMath::Max(InRange, 2000.0f); // 최소 범위 보장
    ConeAngle = FMath::Clamp(InAngle, 30.0f, 120.0f); // 각도 제한 (너무 좁거나 넓지 않게)
    ConeDirection = InDirection.GetSafeNormal();

    // Z 방향 제거하여 수평면에서만 작동하도록
    ConeDirection.Z = 0.0f;
    ConeDirection.Normalize();

    UE_LOG(LogTemp, Warning, TEXT("Cone AOE Setup: Range=%f, Angle=%f, Direction=%s"),
        ConeRange, ConeAngle, *ConeDirection.ToString());

    SetupWarningVisual();
}

void ACircleAOE::SetupAsShareAOE(float InRadius, int32 InMinPlayers)
{
    AOEType = EAOEType::Share;
    Radius = InRadius;
    MinSharePlayers = InMinPlayers;
    bShouldTrackPlayer = true; // 기본값으로 추적 활성화

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }

    // 가장 가까운 플레이어를 추적 대상으로 설정
    if (HasAuthority())
    {
        TArray<AActor*> AllPlayers;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), AllPlayers);

        if (AllPlayers.Num() > 0)
        {
            // 가장 가까운 플레이어 찾기
            AActor* ClosestPlayer = nullptr;
            float ClosestDistance = FLT_MAX;

            for (AActor* Player : AllPlayers)
            {
                float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
                if (Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestPlayer = Player;
                }
            }

            if (ClosestPlayer)
            {
                SetTrackedPlayer(ClosestPlayer);
                UE_LOG(LogTemp, Warning, TEXT("Share AOE: Tracking player %s"), *ClosestPlayer->GetName());
            }
        }
    }

    SetupWarningVisual();
}

void ACircleAOE::SetTrackedPlayer(AActor* Player)
{
    if (HasAuthority())
    {
        TrackedPlayer = Player;
        if (bShouldTrackPlayer && TrackedPlayer)
        {
            StartPlayerTracking();
        }
    }
}

void ACircleAOE::StartPlayerTracking()
{
    if (!HasAuthority() || !TrackedPlayer) return;

    // 0.1초마다 위치 업데이트만 (페널티 데미지 타이머 제거)
    GetWorld()->GetTimerManager().SetTimer(
        TrackingTimerHandle,
        this,
        &ACircleAOE::UpdateTrackingPosition,
        0.1f, // 0.1초마다
        true  // 반복
    );

    UE_LOG(LogTemp, Warning, TEXT("Share AOE: Started tracking player %s (NO PENALTY DAMAGE)"), *TrackedPlayer->GetName());
}

void ACircleAOE::ApplyOutOfRangePenalty()
{
    return;
}

// 추적 위치 업데이트
void ACircleAOE::UpdateTrackingPosition()
{
    if (!HasAuthority() || !TrackedPlayer || !bShouldTrackPlayer) return;

    // 플레이어 위치로 부드럽게 이동
    FVector TargetLocation = TrackedPlayer->GetActorLocation();
    TargetLocation.Z = 9.5f; // 바닥 높이 유지

    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, 0.1f, TrackingSpeed);

    SetActorLocation(NewLocation);

    UE_LOG(LogTemp, Log, TEXT("Share AOE: Tracking player - moved to %s"), *NewLocation.ToString());
}

// 추적 중지
void ACircleAOE::StopPlayerTracking()
{
    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().ClearTimer(TrackingTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("Share AOE: Stopped tracking"));
    }
}

void ACircleAOE::SetupAsConeFromBoss(AActor* BossActor, float InRange, float InAngle)
{
    if (BossActor)
    {
        // 보스에서 랜덤한 플레이어 방향으로 부채꼴 설정
        TArray<AActor*> Players = GetPlayersInRange();
        if (Players.Num() > 0)
        {
            AActor* TargetPlayer = Players[FMath::RandRange(0, Players.Num() - 1)];
            FVector Direction = (TargetPlayer->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal();
            SetupAsConeAOE(InRange, InAngle, Direction);
        }
        else
        {
            // 플레이어가 없으면 보스의 정면 방향
            SetupAsConeAOE(InRange, InAngle, BossActor->GetActorForwardVector());
        }
    }
}

// ========================================
// 시각적 설정 - 타입별 분기
// ========================================

void ACircleAOE::SetupWarningVisual()
{
    switch (AOEType)
    {
    case EAOEType::Circle:
        SetupCircleVisual();
        break;
    case EAOEType::Cone:
        SetupConeVisual();
        break;
    case EAOEType::Share:
        SetupShareVisual();
        break;
    }
}

void ACircleAOE::SetupCircleVisual()
{
    if (WarningMesh)
    {
        // 원형 메시 설정 (기존 코드 유지)
        float ScaleXY = Radius / 50.0f;
        float ScaleZ = 0.1f;
        WarningMesh->SetRelativeScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));

        WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WarningMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
        WarningMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        WarningMesh->SetCanEverAffectNavigation(false);

        UE_LOG(LogTemp, Warning, TEXT("Circle WarningMesh configured: Scale(%f, %f, %f)"),
            ScaleXY, ScaleXY, ScaleZ);
    }

    // Dynamic Material Instance 생성
    if (WarningMaterial && WarningMesh)
    {
        DynamicMaterial = WarningMesh->CreateDynamicMaterialInstance(0, WarningMaterial);
        if (DynamicMaterial)
        {
            UE_LOG(LogTemp, Warning, TEXT("Circle DynamicMaterial created successfully!"));
            DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor(1.0f, 0.5f, 0.0f, 0.8f));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WarningMaterial or WarningMesh is null!"));
    }
}

void ACircleAOE::SetupConeVisual()
{
    if (WarningMesh)
    {
        float ActualConeRange = FMath::Max(ConeRange, 2000.0f);
        float ScaleX = ActualConeRange / 100.0f;
        float ScaleY = ScaleX * 0.8f;
        float ScaleZ = 0.1f;

        WarningMesh->SetRelativeScale3D(FVector(ScaleX, ScaleY, ScaleZ));

        // 회전 설정
        FRotator ConeRotation = ConeDirection.Rotation();
        ConeRotation.Pitch = 0.0f;
        ConeRotation.Roll = 0.0f;
        WarningMesh->SetRelativeRotation(ConeRotation);

        // 충돌 및 렌더링 설정
        WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WarningMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
        WarningMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        WarningMesh->SetCanEverAffectNavigation(false);

        // 강제로 보이게 설정
        WarningMesh->SetVisibility(true);
        WarningMesh->SetHiddenInGame(false);

        // 머티리얼 설정
        UMaterialInterface* MaterialToUse = ConeWarningMaterial ? ConeWarningMaterial : WarningMaterial;
        if (MaterialToUse)
        {
            DynamicMaterial = WarningMesh->CreateDynamicMaterialInstance(0, MaterialToUse);
            if (DynamicMaterial)
            {
                DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor(1.0f, 0.0f, 1.0f, 1.0f));
                DynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0.9f);
                UE_LOG(LogTemp, Warning, TEXT("Cone DynamicMaterial with BRIGHT MAGENTA!"));
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Cone Visual Setup Complete: Location=(0,0,-45), Rotation=(%s)"),
            *ConeRotation.ToString());
    }
}

void ACircleAOE::SetupShareVisual()
{
    if (WarningMesh)
    {
        // 쉐어링은 원형과 비슷하지만 색상이 다름
        float ScaleXY = Radius / 50.0f;
        float ScaleZ = 0.1f;
        WarningMesh->SetRelativeScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));

        WarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WarningMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
        WarningMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        WarningMesh->SetCanEverAffectNavigation(false);

        UE_LOG(LogTemp, Warning, TEXT("Share WarningMesh configured: Scale(%f, %f, %f)"),
            ScaleXY, ScaleXY, ScaleZ);

        // 쉐어링 전용 머티리얼 적용
        UMaterialInterface* MaterialToUse = ShareWarningMaterial ? ShareWarningMaterial : WarningMaterial;
        if (MaterialToUse)
        {
            DynamicMaterial = WarningMesh->CreateDynamicMaterialInstance(0, MaterialToUse);
            if (DynamicMaterial)
            {
                UE_LOG(LogTemp, Warning, TEXT("Share DynamicMaterial created successfully!"));
                DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor(0.0f, 1.0f, 1.0f, 0.8f)); // 청록색 계열
            }
        }
    }
}

void ACircleAOE::OnRep_IsWarningPhase()
{
    if (bIsWarningPhase)
    {
        // 클라이언트에서 경고 시각 효과 시작
        if (WarningMesh)
        {
            WarningMesh->SetVisibility(true);
        }
    }
    else
    {
        // 클라이언트에서 경고 시각 효과 종료
        if (WarningMesh)
        {
            WarningMesh->SetVisibility(false);
        }
    }
}

// Called every frame
void ACircleAOE::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsWarningPhase)
    {
        // 서버에서만 시간 업데이트
        if (HasAuthority())
        {
            ElapsedTime += DeltaTime;
        }

        // 모든 클라이언트에서 색상 애니메이션 업데이트
        float Alpha = ElapsedTime / WarningDuration;
        UpdateColorAnimation(Alpha);
    }
}

void ACircleAOE::StartAOE()
{
    // 서버에서만 실행
    if (!HasAuthority())
        return;

    bIsWarningPhase = true;
    ElapsedTime = 0.0f;

    // Share AOE의 경우 추적 시작
    if (AOEType == EAOEType::Share && bShouldTrackPlayer)
    {
        StartPlayerTracking();
    }

    // 모든 클라이언트에 경고 시작 알림
    MulticastStartWarning();

    // 경고 시간 후 폭발
    GetWorld()->GetTimerManager().SetTimer(
        WarningTimerHandle,
        this,
        &ACircleAOE::Explode,
        WarningDuration,
        false
    );
}

void ACircleAOE::MulticastStartWarning_Implementation()
{
    // 경고 표시
    if (WarningMesh)
    {
        WarningMesh->SetVisibility(true);
        WarningMesh->SetHiddenInGame(false);

        // Cone AOE 특별 처리
        if (AOEType == EAOEType::Cone)
        {
            UE_LOG(LogTemp, Warning, TEXT("CONE AOE: MulticastStartWarning - Visibility forced ON"));

            // 다시 한번 머티리얼 설정
            if (DynamicMaterial)
            {
                DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor(1.0f, 0.0f, 1.0f, 1.0f));
            }
        }
    }
}

void ACircleAOE::Explode()
{
    // 서버에서만 실행
    if (!HasAuthority())
        return;

    bIsWarningPhase = false;

    // 추적 중지
    if (AOEType == EAOEType::Share)
    {
        StopPlayerTracking();
    }

    // 모든 클라이언트에 폭발 알림
    MulticastExplode();

    if (CollisionSphere)
    {
        CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }

    // 범위 내 플레이어에게 데미지 (서버에서만)
    DealDamageToPlayersInRange();

    Destroy();
}

void ACircleAOE::MulticastExplode_Implementation()
{
    // 경고 메시 숨기기
    if (WarningMesh)
    {
        WarningMesh->SetVisibility(false);
    }
}

// ========================================
// 색상 애니메이션 업데이트 - 타입별
// ========================================

void ACircleAOE::UpdateColorAnimation(float Alpha)
{
    switch (AOEType)
    {
    case EAOEType::Circle:
        UpdateCircleAnimation(Alpha);
        break;
    case EAOEType::Cone:
        UpdateConeAnimation(Alpha);
        break;
    case EAOEType::Share:
        UpdateShareAnimation(Alpha);
        break;
    }
}

void ACircleAOE::UpdateCircleAnimation(float Alpha)
{
    if (DynamicMaterial)
    {
        // 기존 원형 애니메이션 로직 유지
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

void ACircleAOE::UpdateConeAnimation(float Alpha)
{
    if (DynamicMaterial)
    {
        // 부채꼴 전용 애니메이션 (더 강렬한 효과)
        FLinearColor BaseColor = FMath::Lerp(
            FLinearColor(1.0f, 0.2f, 0.8f, 0.75f), // 시작: 분홍-보라
            FLinearColor(1.0f, 0.0f, 0.5f, 0.95f), // 끝: 진한 분홍
            Alpha
        );

        // 빠르게 깜빡이는 효과
        float BlinkSpeed = FMath::Lerp(4.0f, 20.0f, Alpha);
        float BlinkValue = (FMath::Sin(ElapsedTime * BlinkSpeed) + 1.0f) * 0.5f;

        FLinearColor FinalColor = BaseColor;
        FinalColor.A *= (0.7f + BlinkValue * 0.3f);

        // 경고가 끝날수록 더 밝게
        if (Alpha > 0.8f)
        {
            FinalColor.R = FMath::Min(FinalColor.R * 1.5f, 1.0f);
            FinalColor.G = FMath::Min(FinalColor.G * 1.3f, 1.0f);
            FinalColor.B = FMath::Min(FinalColor.B * 1.2f, 1.0f);
        }

        DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FinalColor);
    }
}

void ACircleAOE::UpdateShareAnimation(float Alpha)
{
    if (DynamicMaterial)
    {
        // 쉐어링 전용 애니메이션 (청록색 계열, 천천히 깜빡임)
        FLinearColor BaseColor = FMath::Lerp(
            FLinearColor(0.0f, 1.0f, 1.0f, 0.8f),
            FLinearColor(0.0f, 0.5f, 1.0f, 0.9f),
            Alpha
        );

        float BlinkSpeed = FMath::Lerp(2.0f, 8.0f, Alpha);
        float BlinkValue = (FMath::Sin(ElapsedTime * BlinkSpeed) + 1.0f) * 0.5f;

        FLinearColor FinalColor = BaseColor;
        FinalColor.A *= (0.7f + BlinkValue * 0.3f);

        DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FinalColor);
    }
}

// ========================================
// 플레이어 범위 체크 및 데미지 계산
// ========================================

void ACircleAOE::DealDamageToPlayersInRange()
{
    if (AOEType == EAOEType::Share)
    {
        // Share AOE 전용 데미지 처리
        TArray<AActor*> AllPlayers;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), AllPlayers);

        for (AActor* Player : AllPlayers)
        {
            if (ALSPlayer* LSPlayer = Cast<ALSPlayer>(Player))
            {
                float FinalDamage = CalculateShareDamage(Player, AllPlayers);

                if (FinalDamage > 0.0f)
                {
                    FDamageEvent DamageEvent(UDamageType::StaticClass());
                    LSPlayer->TakeDamage(FinalDamage, DamageEvent, nullptr, this);

                    UE_LOG(LogTemp, Warning, TEXT("Share AOE dealt %f damage to %s"),
                        FinalDamage, *LSPlayer->GetName());
                }
            }
        }
    }
    else
    {
        TArray<AActor*> PlayersInRange = GetPlayersInRange();

        for (AActor* Player : PlayersInRange)
        {
            if (ALSPlayer* LSPlayer = Cast<ALSPlayer>(Player))
            {
                float FinalDamage = 0.0f;

                switch (AOEType)
                {
                case EAOEType::Circle:
                    FinalDamage = CalculateCircleDamage(Player);
                    break;
                case EAOEType::Cone:
                    FinalDamage = CalculateConeDamage(Player);
                    break;
                default:
                    break;
                }

                if (FinalDamage > 0.0f)
                {
                    FDamageEvent DamageEvent(UDamageType::StaticClass());
                    LSPlayer->TakeDamage(FinalDamage, DamageEvent, nullptr, this);

                    UE_LOG(LogTemp, Warning, TEXT("%s AOE dealt %f damage to %s"),
                        *UEnum::GetValueAsString(AOEType), FinalDamage, *LSPlayer->GetName());
                }
            }
        }
    }
}

bool ACircleAOE::IsPlayerInCircle(AActor* Player)
{
    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    return Distance <= Radius;
}

bool ACircleAOE::IsPlayerInCone(AActor* Player)
{
    FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();

    // Z축 무시하고 2D 평면에서만 계산
    ToPlayer.Z = 0.0f;
    float Distance = ToPlayer.Size();

    // 범위 체크 (실제 ConeRange 사용)
    if (Distance > ConeRange)
    {
        return false;
    }

    // 각도 체크 (2D 평면에서)
    ToPlayer.Normalize();
    FVector Cone2D = ConeDirection;
    Cone2D.Z = 0.0f;
    Cone2D.Normalize();

    float DotProduct = FVector::DotProduct(Cone2D, ToPlayer);

    // DotProduct가 -1~1 범위를 벗어나지 않도록 클램프
    DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);

    float AngleInRadians = FMath::Acos(DotProduct);
    float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

    bool bIsInCone = AngleInDegrees <= (ConeAngle * 0.5f);

    if (bIsInCone)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %s is in cone: Distance=%f, Angle=%f"),
            *Player->GetName(), Distance, AngleInDegrees);
    }

    return bIsInCone;
}

bool ACircleAOE::IsPlayerInShare(AActor* Player)
{
    // 쉐어링은 원형과 동일한 범위 체크
    return IsPlayerInCircle(Player);
}

float ACircleAOE::CalculateCircleDamage(AActor* Player)
{
    return IsPlayerInCircle(Player) ? Damage : 0.0f;
}

float ACircleAOE::CalculateConeDamage(AActor* Player)
{
    return IsPlayerInCone(Player) ? Damage : 0.0f;
}

float ACircleAOE::CalculateShareDamage(AActor* Player, const TArray<AActor*>& PlayersInRange)
{
    if (!IsPlayerInShare(Player))
    {
        // 폭발 시 범위 밖에 있으면 즉사급 데미지
        UE_LOG(LogTemp, Warning, TEXT("Share AOE EXPLODE: %s is OUT OF SHARE RANGE when exploded - FATAL DAMAGE!"), *Player->GetName());
        return MaxSoloFatalDamage;
    }

    // 범위 안에 있는 플레이어 수 계산
    int32 PlayersInShareRange = 0;
    for (AActor* CheckPlayer : PlayersInRange)
    {
        if (IsPlayerInShare(CheckPlayer))
            PlayersInShareRange++;
    }

    if (PlayersInShareRange < MinSharePlayers)
    {
        // 범위 안에 있지만 인원이 부족하면 즉사급 데미지
        UE_LOG(LogTemp, Warning, TEXT("Share AOE EXPLODE: %s is in range but not enough total players! (%d/%d) - Fatal damage!"),
            *Player->GetName(), PlayersInShareRange, MinSharePlayers);
        return MaxSoloFatalDamage;
    }
    else
    {
        // 인원이 충분하면 분산 데미지
        float SharedDamage = Damage / PlayersInShareRange * ShareDamageReduction;
        UE_LOG(LogTemp, Warning, TEXT("Share AOE EXPLODE: %s - Damage shared among %d players - %f damage each"),
            *Player->GetName(), PlayersInShareRange, SharedDamage);
        return SharedDamage;
    }
}

// ========================================
// 유틸리티 함수들
// ========================================

TArray<AActor*> ACircleAOE::GetPlayersInRange()
{
    TArray<AActor*> AllPlayers;
    TArray<AActor*> PlayersInRange;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), AllPlayers);

    for (AActor* Player : AllPlayers)
    {
        bool bInRange = false;

        switch (AOEType)
        {
        case EAOEType::Circle:
            bInRange = IsPlayerInCircle(Player);
            break;
        case EAOEType::Cone:
            bInRange = IsPlayerInCone(Player);
            break;
        case EAOEType::Share:
            bInRange = IsPlayerInShare(Player);
            break;
        }

        if (bInRange)
        {
            PlayersInRange.Add(Player);
        }
    }

    return PlayersInRange;
}

float ACircleAOE::GetAngleBetweenVectors(FVector A, FVector B)
{
    A.Normalize();
    B.Normalize();
    float DotProduct = FVector::DotProduct(A, B);
    return FMath::RadiansToDegrees(FMath::Acos(DotProduct));
}