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
    for (int i = 0; i < 5; ++i)
    {
        FString Name = FString::Printf(TEXT("SpawnPoint_%d"), i);
        USceneComponent* SpawnPoint = CreateDefaultSubobject<USceneComponent>(*Name);
        SpawnPoint->SetupAttachment(RootComponent);
        float YOffset = (i - 2) * 82.0f;
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
        NewHP = 40.0f;
        CurrentPhase = 3;
        bIsPhaseChanging = true;
        DamageMontagePlay();
        DestroyObstacles();
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

    // 1단계: 모든 플레이어에게 AOE 하나씩 할당
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), FoundPlayers);

    int32 PlayerAOECount = 0;

    // 각 플레이어마다 AOE 하나씩
    for (AActor* Player : FoundPlayers)
    {
        FVector PlayerLocation = Player->GetActorLocation();

        // 플레이어 근처에 스폰 (피할 수 있을 정도의 오프셋)
        float OffsetDistance = FMath::RandRange(100.0f, 200.0f);
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

        SpawnSingleAOE(SpawnLocation, FString::Printf(TEXT("Player-Target AOE")));
        PlayerAOECount++;
    }

    // 2단계: 추가 랜덤 위치 AOE 생성 (긴장감 조성)
    int32 ExtraRandomAOEs = MaxAOECount;

    for (int32 i = 0; i < ExtraRandomAOEs; ++i)
    {
        FVector RandomLocation = GetRandomLocationAroundBoss();
        SpawnSingleAOE(RandomLocation, FString::Printf(TEXT("Random AOE %d"), i + 1));
    }

    UE_LOG(LogTemp, Warning, TEXT("Spawned %d Player-Target AOEs + %d Random AOEs"),
        PlayerAOECount, ExtraRandomAOEs);
}

// AOE 멀티캐스트 스폰 (핵심 로직)
void ABossNPC::MultiSpawnAOEAttack_Implementation()
{
    
}

// 단일 AOE 스폰 헬퍼 함수
void ABossNPC::SpawnSingleAOE(FVector SpawnLocation, FString AOEType)
{
    ServerSpawnSingleAOE(SpawnLocation, AOEType);
}

void ABossNPC::ServerSpawnSingleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    MultiSpawnSingleAOE(SpawnLocation, AOEType);
}

void ABossNPC::MultiSpawnSingleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
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
        SpawnedAOE->StartAOE();
        UE_LOG(LogTemp, Warning, TEXT("%s spawned at location: %s"),
            *AOEType, *SpawnLocation.ToString());
    }
}

// 장애물 스폰
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

// 플랫폼 스폰
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

// 보스 주변 랜덤 위치 생성
FVector ABossNPC::GetRandomLocationAroundBoss()
{
    FVector BossLocation = GetActorLocation();
    float MinSafeDistance = 400.0f;
    float MaxDistance = 800.0f;

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