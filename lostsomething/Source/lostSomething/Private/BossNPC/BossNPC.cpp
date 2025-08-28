// Fill out your copyright notice in the Description page of Project Settings.

#include "BossNPC/BossNPC.h"
#include "AOE/ACircleAOE.h"
#include "BossNPC/AI/BossNPCAIController.h"
#include "BossNPC/Obstacle/BossObstacle.h"
#include "BossNPC/Platform/PlatformGenerator.h"
#include "Kismet/GameplayStatics.h" 
#include "Character/Players/LSPlayer.h"  
#include "lostSomething.h"
#include <BossNPC/Anim/BossNPCAnimIns.h>

// Sets default values
ABossNPC::ABossNPC()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    // AIController 클래스를 설정
    AIControllerClass = ABossNPCAIController::StaticClass();

    // AIController가 자동으로 소환되도록 설정
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 예시: SceneComponent 생성
    for (int i = 0; i < 7; ++i)
    {
        FString Name = FString::Printf(TEXT("SpawnPoint_%d"), i);
        USceneComponent* SpawnPoint = CreateDefaultSubobject<USceneComponent>(*Name);
        SpawnPoint->SetupAttachment(RootComponent);
        float YOffset = (i - 3) * 41.0f;
        SpawnPoint->SetRelativeLocation(FVector(110.f, YOffset, -45.f));
        ObstacleSpawnPoints.Add(SpawnPoint);
    }


    CurrentHP = MaxHP;

    // AOE 관련 기본값 설정
    AOEAttackInterval = 10.0f;
    MaxAOECount = 2;
    AOESpawnRadius = 500.0f;
    CurrentPhase = 1;
    bIsPhaseChanging = false;
}

// Called when the game starts or when spawned
void ABossNPC::BeginPlay()
{
    Super::BeginPlay();
    //EnterPhase1();
    ABossNPCAIController* PC = Cast<ABossNPCAIController>(GetController());
    if (PC)
    {
        PC->ChangedHP();
    }
}

// Called every frame
void ABossNPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABossNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ABossNPC::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    SetHP(GetHP() - DamageAmount);
    //LS_LOG(LogLS, Log, TEXT("CurrentHP : %f"), CurrentHP)

    ABossNPCAIController* PC = Cast<ABossNPCAIController>(GetController());
    if (PC)
    {
        PC->ChangedHP();
    }

    if (GetHP() <= 0.0f)
    {
        //LS_LOG(LogLS, Log, TEXT("DieMontagePlay"))
        DieMontagePlay();
    }

    return 0.0f;
}

void ABossNPC::SetHP(float NewHP)
{
    if (bIsPhaseChanging)
    {
        // 무적 상태이면 체력 변경 무시
        return;
    }

    if (CurrentPhase == 1 && NewHP <= 80.0f)
    {
        NewHP = 80.0f;
        CurrentPhase = 2;
        bIsPhaseChanging = true;
        DamageMontagePlay();
    }
    else if (CurrentPhase == 2 && NewHP <= 40.0f)
    {
        DestroyObstacles();
        NewHP = 40.0f;
        CurrentPhase = 3;
        bIsPhaseChanging = true;
        DamageMontagePlay();
    }

    CurrentHP = (NewHP < 0.0f) ? 0.0f : NewHP;
}

// 1페이즈 진입 - AOE 패턴
void ABossNPC::EnterPhase1()
{
    //UE_LOG(LogTemp, Warning, TEXT("ABossNPC::EnterPhase1()"));
    if (HasAuthority())
    {
        //CurrentPhase = 1;
        //UE_LOG(LogTemp, Warning, TEXT("Boss entered Phase 1 - AOE Pattern"));
        StartAOEAttackPattern();
    }
}

// 2페이즈 진입 - 장애물 패턴
void ABossNPC::EnterPhase2()
{
    if (HasAuthority())
    {
        //CurrentPhase = 2;
        //UE_LOG(LogTemp, Warning, TEXT("Boss entered Phase 2 - Obstacle Pattern"));

        // 장애물 스폰 패턴 시작
        SpawnObstacles();
    }
}

// 3페이즈 진입 - 플랫폼 생성
void ABossNPC::EnterPhase3()
{
    if (HasAuthority() && PlatformGeneratorClass)
    {
        //CurrentPhase = 3;
        //UE_LOG(LogTemp, Warning, TEXT("Entering Phase 3 - Spawning Platform Generator"));

        FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 0);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        PlatformGenerator = GetWorld()->SpawnActor<APlatformGenerator>(PlatformGeneratorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

        if (PlatformGenerator)
        {
            //UE_LOG(LogTemp, Warning, TEXT("PlatformGenerator spawned successfully"));
            SpawnPlatform();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn PlatformGenerator"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnterPhase3 skipped - No Authority or GeneratorClass is null"));
    }
}

void ABossNPC::AOEMontagePlay()
{
    ServerAOEMontagePlay();
}

void ABossNPC::ServerAOEMontagePlay_Implementation()
{
    MultiAOEMontagePlay();
}

void ABossNPC::MultiAOEMontagePlay_Implementation()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UBossNPCAnimIns* NPCAnimInstance = Cast<UBossNPCAnimIns>(AnimInstance);
    if (!NPCAnimInstance || !NPCAnimInstance->AOEMontage) return;

    //UE_LOG(LogTemp, Warning, TEXT("Bpss AnimInstace exist!"));

    // 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
    if (NPCAnimInstance->AOEMontage)
    {
        NPCAnimInstance->MontagePlay(NPCAnimInstance->AOEMontage);
        //UE_LOG(LogTemp, Warning, TEXT("ABossNPC::AOEMontagePlay()-> MontagePlay"));
    }
}

void ABossNPC::ObsMontagePlay()
{
    ServerObsMontagePlay();
}

void ABossNPC::ServerObsMontagePlay_Implementation()
{
    MultiObsMontagePlay();
}

void ABossNPC::MultiObsMontagePlay_Implementation()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UBossNPCAnimIns* NPCAnimInstance = Cast<UBossNPCAnimIns>(AnimInstance);
    if (!NPCAnimInstance || !NPCAnimInstance->ObstacleMontage) return;

    //UE_LOG(LogTemp, Warning, TEXT("Bpss AnimInstace exist!"));

    // 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
    if (NPCAnimInstance->ObstacleMontage)
    {
        NPCAnimInstance->MontagePlay(NPCAnimInstance->ObstacleMontage);
        //UE_LOG(LogTemp, Warning, TEXT("ABossNPC::AOEMontagePlay()-> MontagePlay"));
    }
}

void ABossNPC::MazeMontagePlay()
{
    ServerMazeMontagePlay();
}

void ABossNPC::ServerMazeMontagePlay_Implementation()
{
    MultiMazeMontagePlay();
}

void ABossNPC::MultiMazeMontagePlay_Implementation()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UBossNPCAnimIns* NPCAnimInstance = Cast<UBossNPCAnimIns>(AnimInstance);
    if (!NPCAnimInstance || !NPCAnimInstance->MazeMontage) return;

    //UE_LOG(LogTemp, Warning, TEXT("Bpss AnimInstace exist!"));

    // 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
    if (NPCAnimInstance->MazeMontage)
    {
        NPCAnimInstance->MontagePlay(NPCAnimInstance->MazeMontage);
        //UE_LOG(LogTemp, Warning, TEXT("ABossNPC::AOEMontagePlay()-> MontagePlay"));
    }
}

void ABossNPC::DamageMontagePlay()
{
    ServerDamageMontagePlay();
}

void ABossNPC::ServerDamageMontagePlay_Implementation()
{
    MultiDamageMontagePlay();
}

void ABossNPC::MultiDamageMontagePlay_Implementation()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UBossNPCAnimIns* NPCAnimInstance = Cast<UBossNPCAnimIns>(AnimInstance);
    if (!NPCAnimInstance || !NPCAnimInstance->DamageMontage) return;

    // 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
    if (NPCAnimInstance->DamageMontage)
    {
        NPCAnimInstance->MontagePlay(NPCAnimInstance->DamageMontage);
    }
}

void ABossNPC::DieMontagePlay()
{
    ServerDieMontagePlay();
}

void ABossNPC::ServerDieMontagePlay_Implementation()
{
    MultiDieMontagePlay();
}

void ABossNPC::MultiDieMontagePlay_Implementation()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UBossNPCAnimIns* NPCAnimInstance = Cast<UBossNPCAnimIns>(AnimInstance);
    if (!NPCAnimInstance || !NPCAnimInstance->DeathMontage) return;

    // 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
    if (NPCAnimInstance->DeathMontage)
    {
        NPCAnimInstance->MontagePlay(NPCAnimInstance->DeathMontage);
    }
}

void ABossNPC::Despawn()
{
    ServerDespawn();
}

void ABossNPC::ServerDespawn_Implementation()
{
    MultiDespawn();
}

void ABossNPC::MultiDespawn_Implementation()
{
    SetLifeSpan(3.0f);
}

// AOE 공격 패턴 시작
void ABossNPC::StartAOEAttackPattern()
{
    if (HasAuthority())
    {
        // 첫 AOE는 즉시 시작
        SpawnAOEAttack();

        /*GetWorld()->GetTimerManager().SetTimer(
            AOEPatternTimerHandle,
            this,
            &ABossNPC::SpawnAOEAttack,
            AOEAttackInterval,
            true  // 반복
        );*/
        // 이후 주기적으로 AOE 스폰


        UE_LOG(LogTemp, Warning, TEXT("AOE Attack Pattern Started"));
    }
}

// AOE 스폰
void ABossNPC::SpawnAOEAttack()
{
    ServerSpawnAOEAttack();
}

void ABossNPC::ServerSpawnAOEAttack_Implementation()
{
    if (!CircleAOEClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CircleAOEClass is not set!"));
        return;
    }

    // 1단계: 플레이어 정보 수집
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), FoundPlayers);

    // 2단계: 쉐어 AOE 플래그 (전체에서 최대 1개만)
    bool bHasSpawnedShareAOE = false;

    // 3단계: 부채꼴 AOE 최대 개수 제한 (맵 크기 고려)
    int32 MaxConeAOEs = 1; // 부채꼴은 최대 1개만
    int32 SpawnedConeAOEs = 0;

    int32 PlayerAOECount = 0;

    // 4단계: 각 플레이어마다 AOE 하나씩 (확률 조정)
    for (AActor* Player : FoundPlayers)
    {
        FVector PlayerLocation = Player->GetActorLocation();

        // 플레이어 근처에 스폰 (피할 수 있을 정도의 오프셋)
        float OffsetDistance = FMath::RandRange(PlayerTargetOffset_Min, PlayerTargetOffset_Max);
        float OffsetAngle = FMath::RandRange(0.0f, 360.0f);

        FVector Offset = FVector(
            FMath::Cos(FMath::DegreesToRadians(OffsetAngle)) * OffsetDistance,
            FMath::Sin(FMath::DegreesToRadians(OffsetAngle)) * OffsetDistance,
            0.0f
        );

        FVector SpawnLocation = FVector(
            PlayerLocation.X + Offset.X,
            PlayerLocation.Y + Offset.Y,
            9.5f
        );

        // 수정된 확률 시스템 (85% 원형, 15% 쉐어링, 부채꼴은 따로 처리)
        int32 RandomChance = FMath::RandRange(1, 100);

        if (RandomChance <= 85)
        {
            // 기본 원형 AOE
            SpawnSingleCircleAOE(SpawnLocation, FString::Printf(TEXT("Player-Target Circle AOE")));
        }
        else if (RandomChance <= 100 && !bHasSpawnedShareAOE)
        {
            // 쉐어링 AOE (전체에서 최대 1개만)
            SpawnSingleShareAOE(SpawnLocation, FString::Printf(TEXT("Player-Target Share AOE")));
            bHasSpawnedShareAOE = true;
        }
        else
        {
            // 쉐어 AOE가 이미 있으면 원형 AOE로 대체
            SpawnSingleCircleAOE(SpawnLocation, FString::Printf(TEXT("Player-Target Circle AOE (Share Fallback)")));
        }

        PlayerAOECount++;
    }

    // 5단계: 추가 랜덤 위치 AOE 생성
    int32 ExtraRandomAOEs = MaxAOECount;

    for (int32 i = 0; i < ExtraRandomAOEs; ++i)
    {
        FVector RandomLocation = GetRandomLocationAroundBoss();

        // 수정된 확률 (90% 원형, 10% 쉐어링, 부채꼴은 따로 처리)
        int32 RandomChance = FMath::RandRange(1, 100);

        if (RandomChance <= 90)
        {
            SpawnSingleCircleAOE(RandomLocation, FString::Printf(TEXT("Random Circle AOE %d"), i + 1));
        }
        else if (RandomChance <= 100 && !bHasSpawnedShareAOE)
        {
            // 쉐어링 AOE (전체에서 최대 1개만)
            SpawnSingleShareAOE(RandomLocation, FString::Printf(TEXT("Random Share AOE %d"), i + 1));
            bHasSpawnedShareAOE = true;
        }
        else
        {
            // 쉐어 AOE가 이미 있으면 원형 AOE로 대체
            SpawnSingleCircleAOE(RandomLocation, FString::Printf(TEXT("Random Circle AOE %d (Share Fallback)"), i + 1));
        }
    }

    // 6단계: 부채꼴 AOE 별도 처리 (보스 근처에서만, 30% 확률)
    int32 ConeChance = FMath::RandRange(1, 100);
    if (ConeChance <= 30 && SpawnedConeAOEs < MaxConeAOEs)
    {
        // 보스 근처의 플레이어를 타겟으로 하는 부채꼴
        TArray<AActor*> NearbyPlayers;
        float MaxConeDistance = 800.0f; // 보스 주변 800 유닛 내의 플레이어만 타겟

        for (AActor* Player : FoundPlayers)
        {
            float DistanceToBoss = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
            if (DistanceToBoss <= MaxConeDistance)
            {
                NearbyPlayers.Add(Player);
            }
        }

        if (NearbyPlayers.Num() > 0)
        {
            // 근처 플레이어 중 랜덤 선택하여 방향 결정
            AActor* TargetPlayer = NearbyPlayers[FMath::RandRange(0, NearbyPlayers.Num() - 1)];
            FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

            SpawnSingleConeAOE(GetActorLocation(), Direction, FString::Printf(TEXT("Boss Cone AOE")));
            SpawnedConeAOEs++;
        }
        else
        {
            // 근처에 플레이어가 없으면 랜덤 방향
            float RandomAngle = FMath::RandRange(0.0f, 360.0f);
            FVector Direction = FVector(
                FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
                FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
                0.0f
            );
            SpawnSingleConeAOE(GetActorLocation(), Direction, FString::Printf(TEXT("Boss Random Cone AOE")));
            SpawnedConeAOEs++;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("AOE Spawn Summary: %d Player-Target AOEs, %d Random AOEs, %d Cone AOEs, Share AOE: %s"),
        PlayerAOECount, ExtraRandomAOEs, SpawnedConeAOEs, bHasSpawnedShareAOE ? TEXT("YES") : TEXT("NO"));
}

// AOE 멀티캐스트 스폰 (핵심 로직)
void ABossNPC::MultiSpawnAOEAttack_Implementation()
{

}

// ========================================
// Circle AOE 스폰 함수들
// ========================================

void ABossNPC::SpawnSingleCircleAOE(FVector SpawnLocation, FString AOEType)
{
    ServerSpawnSingleCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::ServerSpawnSingleCircleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    MultiSpawnSingleCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::MultiSpawnSingleCircleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ACircleAOE* SpawnedAOE = GetWorld()->SpawnActor<ACircleAOE>(
        CircleAOEClass,
        SpawnLocation,
        FRotator::ZeroRotator,
        Params
    );

    if (SpawnedAOE)
    {
        SpawnedAOE->SetupAsCircleAOE(250.0f); // 기본 원형 AOE
        SpawnedAOE->StartAOE();
        UE_LOG(LogTemp, Warning, TEXT("%s spawned at location: %s"),
            *AOEType, *SpawnLocation.ToString());
    }
}

// ========================================
// Share AOE 스폰 함수들
// ========================================

void ABossNPC::SpawnSingleShareAOE(FVector SpawnLocation, FString AOEType)
{
    ServerSpawnSingleShareAOE(SpawnLocation, AOEType);
}

void ABossNPC::ServerSpawnSingleShareAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    MultiSpawnSingleShareAOE(SpawnLocation, AOEType);
}

void ABossNPC::MultiSpawnSingleShareAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ACircleAOE* SpawnedAOE = GetWorld()->SpawnActor<ACircleAOE>(
        CircleAOEClass,
        SpawnLocation,
        FRotator::ZeroRotator,
        Params
    );

    if (SpawnedAOE)
    {
        SpawnedAOE->SetupAsShareAOE(300.0f, 2); // 반지름 300, 최소 2명 필요
        SpawnedAOE->StartAOE();
        UE_LOG(LogTemp, Warning, TEXT("%s spawned at location: %s - SHARE MECHANICS! (Need 2+ players)"),
            *AOEType, *SpawnLocation.ToString());
    }
}

// ========================================
// Cone AOE 스폰 함수들
// ========================================

void ABossNPC::SpawnSingleConeAOE(FVector SpawnLocation, FVector Direction, FString AOEType)
{
    ServerSpawnSingleConeAOE(SpawnLocation, Direction, AOEType);
}

void ABossNPC::ServerSpawnSingleConeAOE_Implementation(FVector SpawnLocation, FVector Direction, const FString& AOEType)
{
    MultiSpawnSingleConeAOE(SpawnLocation, Direction, AOEType);
}

void ABossNPC::MultiSpawnSingleConeAOE_Implementation(FVector SpawnLocation, FVector Direction, const FString& AOEType)
{
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 부채꼴은 항상 보스 위치에서 시작
    FVector BossLocation = GetActorLocation();
    BossLocation.Z = 9.5f; // 바닥 높이에 정확히 맞추기

    ACircleAOE* SpawnedAOE = GetWorld()->SpawnActor<ACircleAOE>(
        CircleAOEClass,
        BossLocation,
        FRotator::ZeroRotator, // 회전은 SetupAsConeAOE에서 처리
        Params
    );

    if (SpawnedAOE)
    {
        // 방향을 수평면으로 정규화
        FVector NormalizedDirection = Direction;
        NormalizedDirection.Z = 0.0f;
        NormalizedDirection.Normalize();

        // 맵 크기에 맞는 충분한 범위 설정 (3000 유닛으로 증가)
        float ConeRange = 3000.0f;
        float ConeAngle = 75.0f; // 적당한 각도로 조정

        SpawnedAOE->SetupAsConeAOE(ConeRange, ConeAngle, NormalizedDirection);
        SpawnedAOE->StartAOE();

        UE_LOG(LogTemp, Warning, TEXT("%s spawned at BOSS location: %s - IMPROVED CONE ATTACK! Direction: %s, Range: %f, Angle: %f"),
            *AOEType, *BossLocation.ToString(), *NormalizedDirection.ToString(), ConeRange, ConeAngle);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn Cone AOE!"));
 
    
    
    }
}

TArray<AActor*> ABossNPC::GetNearbyPlayers(float MaxDistance)
{
    TArray<AActor*> AllPlayers;
    TArray<AActor*> NearbyPlayers;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), AllPlayers);

    FVector BossLocation = GetActorLocation();

    for (AActor* Player : AllPlayers)
    {
        float Distance = FVector::Dist(BossLocation, Player->GetActorLocation());
        if (Distance <= MaxDistance)
        {
            NearbyPlayers.Add(Player);
            UE_LOG(LogTemp, Log, TEXT("Player %s is nearby (Distance: %f)"), *Player->GetName(), Distance);
        }
    }

    return NearbyPlayers;
}

// ========================================
// 기존 SpawnSingleAOE 함수들 (호환성 유지)
// ========================================

void ABossNPC::SpawnSingleAOE(FVector SpawnLocation, FString AOEType)
{
    // 기존 코드와의 호환성을 위해 Circle AOE로 처리
    SpawnSingleCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::ServerSpawnSingleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    // 기존 코드와의 호환성을 위해 Circle AOE로 처리
    ServerSpawnSingleCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::MultiSpawnSingleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    // 기존 코드와의 호환성을 위해 Circle AOE로 처리
    MultiSpawnSingleCircleAOE(SpawnLocation, AOEType);
}

// ========================================
// 장애물 관련 함수들 (기존 그대로 유지)
// ========================================

void ABossNPC::SpawnObstacles()
{
    ServerSpawnObstacles();
}

void ABossNPC::ServerSpawnObstacles_Implementation()
{
    int32 SpawnPointCount = ObstacleSpawnPoints.Num();

    if (SpawnPointCount == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("ObstacleSpawnPoints is empty!"));
        return;
    }

    // 동적으로 인덱스 리스트 생성
    TArray<int32> Indexes;
    for (int32 i = 0; i < SpawnPointCount; ++i)
    {
        Indexes.Add(i);
    }

    // 랜덤 셔플
    for (int32 i = 0; i < Indexes.Num(); ++i)
    {
        int32 RandIdx = FMath::RandRange(i, Indexes.Num() - 1);
        Indexes.Swap(i, RandIdx);
    }

    // 3 또는 4개 스폰하려 했지만, 그 수보다 SpawnPoint가 적으면 문제 발생
    // 따라서 Clamp 필요
    int32 NumToSpawn = FMath::Clamp(FMath::RandBool() ? 3 : 4, 1, SpawnPointCount);

    for (int32 i = 0; i < NumToSpawn; ++i)
    {
        USceneComponent* SpawnPoint = ObstacleSpawnPoints[Indexes[i]];
        if (!SpawnPoint) continue;

        FVector SpawnLocation = SpawnPoint->GetComponentLocation();
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters Params;
        Params.Owner = this;

        SpawnedObstacles.Empty();

        ABossObstacle* NewObstacle = GetWorld()->SpawnActor<ABossObstacle>(
            ABossObstacle::StaticClass(),
            SpawnLocation,
            SpawnRotation,
            Params
        );

        if (NewObstacle)
        {
            SpawnedObstacles.Add(NewObstacle);
        }
    }
}

void ABossNPC::DestroyObstacles()
{
    ServerDestroyObstacles();
}

void ABossNPC::ServerDestroyObstacles_Implementation()
{
    MultiDestroyObstacles();
}

void ABossNPC::MultiDestroyObstacles_Implementation()
{
    for (ABossObstacle* Obstacle : SpawnedObstacles)
    {
        if (Obstacle)
        {
            Obstacle->Destroy();
        }
    }
    SpawnedObstacles.Empty(); // 배열 비움
}

// ========================================
// 플랫폼 관련 함수들 (기존 그대로 유지)
// ========================================

void ABossNPC::SpawnPlatform()
{
    if (HasAuthority()) // 서버인지 확인
    {
        if (PlatformGenerator)
        {
            PlatformGenerator->GenerateMaze(); // 핵심 로직 호출
        }
    }
    else
    {
        ServerSpawnPlatform(); // 클라이언트 경우 서버 RPC 요청
    }
}

void ABossNPC::ServerSpawnPlatform_Implementation()
{
    SpawnPlatform();
}

// ========================================
// 유틸리티 함수들 (기존 그대로 유지)
// ========================================

// 보스 주변 랜덤 위치 생성
FVector ABossNPC::GetRandomLocationAroundBoss()
{
    FVector BossLocation = GetActorLocation();

    // Cone AOE가 나올 가능성을 고려하여 보스 근처에는 다른 AOE를 덜 배치
    float MinSafeDistance = 600.0f; // 거리를 늘려서 Cone AOE와 겹치지 않게
    float MaxDistance = 1000.0f;

    float RandomAngle = FMath::RandRange(0.0f, 360.0f);
    float RandomDistance = FMath::RandRange(MinSafeDistance, MaxDistance);

    float RadianAngle = FMath::DegreesToRadians(RandomAngle);
    FVector RandomOffset = FVector(
        FMath::Cos(RadianAngle) * RandomDistance,
        FMath::Sin(RadianAngle) * RandomDistance,
        0.0f
    );

    FVector TargetLocation = BossLocation + RandomOffset;
    return FVector(TargetLocation.X, TargetLocation.Y, 9.5f);
}

// AOE 패턴 정지
void ABossNPC::StopAOEPattern()
{
    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().ClearTimer(AOEPatternTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("AOE Pattern Stopped"));
    }
}

// 장애물 패턴 정지
void ABossNPC::StopObstaclePattern()
{
    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().ClearTimer(ObstaclePatternTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("Obstacle Pattern Stopped"));
    }
}

// 네트워크 복제 설정
void ABossNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //DOREPLIFETIME(ABossNPC, CurrentPhase);
}