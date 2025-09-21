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
#include <UObject/FastReferenceCollector.h>

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

void ABossNPC::BMSoundPlay(const FString& SoundType)
{
    ServerBSoundPlay(SoundType);
}

void ABossNPC::ServerBSoundPlay_Implementation(const FString& SoundType)
{
    MultiBMSoundPlay(SoundType);
}

void ABossNPC::MultiBMSoundPlay_Implementation(const FString& SoundType)
{
    USoundBase* SelectedSound = nullptr;

    if (SoundType == "Phase1") SelectedSound = Phase1Sound;
    else if (SoundType == "Phase2") SelectedSound = Phase2Sound;
    else if (SoundType == "Phase3") SelectedSound = Phase3Sound;
    else if (SoundType == "Damage") SelectedSound = DamageSound;
    else if (SoundType == "Die") SelectedSound = DieSound;
    else if (SoundType == "HP") SelectedSound = HPChangeSound;
    if (!SelectedSound)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sound is nullptr for type %s"), *SoundType);
        return;
    }

    // 핵심: GetWorld() 기반으로 2D 사운드 재생
    UGameplayStatics::PlaySound2D(GetWorld(), SelectedSound, 0.1f);
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
    BMSoundPlay("HP");

    ABossNPCAIController* PC = Cast<ABossNPCAIController>(GetController());
    if (PC)
    {
        PC->ChangedHP();
    }

    if (GetHP() <= 0.0f)
    {
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
        HideGroupActor();
    }
    CurrentHP = (NewHP < 0.0f) ? 0.0f : NewHP;
}

// 1페이즈 진입 - AOE 패턴
void ABossNPC::EnterPhase1()
{
    if (HasAuthority())
    {
        StartAOEAttackPattern();
    }
}

// 2페이즈 진입 - 장애물 패턴
void ABossNPC::EnterPhase2()
{
    if (HasAuthority())
    {
        // 장애물 스폰 패턴 시작
        SpawnObstacles();
    }
}

// 3페이즈 진입 - 플랫폼 생성
void ABossNPC::EnterPhase3()
{
    if (HasAuthority())
    {
        if (PlatformGeneratorClass)
        {
            FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 0);
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            PlatformGenerator = GetWorld()->SpawnActor<APlatformGenerator>(PlatformGeneratorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

            if (PlatformGenerator)
            {
                SpawnPlatform();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn PlatformGenerator"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnterPhase3 skipped - No Authority or GeneratorClass is null"));
    }
}

void ABossNPC::HideGroupActor()
{
    if (HasAuthority())
    {
        ServerHideGroupActor();
    }
}

void ABossNPC::ServerHideGroupActor_Implementation()
{
    MultiHideGroupActor();
}

void ABossNPC::MultiHideGroupActor_Implementation()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("GroupActor")), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (Actor)
        {
            Actor->SetActorHiddenInGame(true);
            Actor->SetActorEnableCollision(false);
        }
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

    // 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
    if (NPCAnimInstance->AOEMontage)
    {
        NPCAnimInstance->MontagePlay(NPCAnimInstance->AOEMontage);
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
    }
}

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

    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), FoundPlayers);

    bool bShouldSpawnLargeAOE = false;
    int32 PlayersInDangerZone = 0;

    for (AActor* Player : FoundPlayers)
    {
        FVector PlayerLocation = Player->GetActorLocation();
        float PlayerY = PlayerLocation.Y;

        if (PlayerY >= -3885.69f && PlayerY <= -3455.69f)
        {
            PlayersInDangerZone++;
        }
    }

    if (PlayersInDangerZone > 0)
    {
        int32 HighChance = FMath::RandRange(1, 100);
        if (HighChance <= 60)
        {
            bShouldSpawnLargeAOE = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%d players in danger zone - HIGH chance Large AOE failed (20%% miss)"), PlayersInDangerZone);
        }
    }
    else
    {
        int32 LowChance = FMath::RandRange(1, 100);
        if (LowChance <= 10)
        {
            bShouldSpawnLargeAOE = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No players in danger zone - LOW chance Large AOE not triggered"));
        }
    }

    if (bShouldSpawnLargeAOE)
    {
        FVector FixedLargeAOELocation = FVector(-33.409138f, -4088.049805f, 9.5f);

        SpawnSingleLargeCircleAOE(FixedLargeAOELocation, FString::Printf(TEXT("Fixed Position Large Circle AOE")));

        //UE_LOG(LogTemp, Warning, TEXT("Spawned LARGE Circle AOE at FIXED position: %s"), *FixedLargeAOELocation.ToString());
        //UE_LOG(LogTemp, Warning, TEXT("AOE Spawn Summary: LARGE AOE ONLY - No other AOEs spawned"));
        return; 
    }

    bool bHasSpawnedShareAOE = false;
    int32 PlayerAOECount = 0;

    for (AActor* Player : FoundPlayers)
    {
        FVector PlayerLocation = Player->GetActorLocation();

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

        int32 RandomChance = FMath::RandRange(1, 100);

        if (RandomChance <= 85)
        {
            SpawnSingleCircleAOE(SpawnLocation, FString::Printf(TEXT("Player-Target Circle AOE")));
        }
        else if (RandomChance <= 100 && !bHasSpawnedShareAOE)
        {
            SpawnSingleShareAOE(SpawnLocation, FString::Printf(TEXT("Player-Target Share AOE")));
            bHasSpawnedShareAOE = true;
        }
        else
        {
            SpawnSingleCircleAOE(SpawnLocation, FString::Printf(TEXT("Player-Target Circle AOE (Share Fallback)")));
        }

        PlayerAOECount++;
    }

    int32 ExtraRandomAOEs = MaxAOECount;
    //UE_LOG(LogTemp, Warning, TEXT("Generating %d extra random AOEs"), ExtraRandomAOEs);

    for (int32 i = 0; i < ExtraRandomAOEs; ++i)
    {
        FVector RandomLocation = GetSafeRandomLocationAroundBoss();
        //UE_LOG(LogTemp, Warning, TEXT("Random AOE %d location: %s"), i + 1, *RandomLocation.ToString());

        int32 RandomChance = FMath::RandRange(1, 100);

        if (RandomChance <= 90)
        {
            SpawnSingleCircleAOE(RandomLocation, FString::Printf(TEXT("Random Circle AOE %d"), i + 1));
        }
        else if (RandomChance <= 100 && !bHasSpawnedShareAOE)
        {
            SpawnSingleShareAOE(RandomLocation, FString::Printf(TEXT("Random Share AOE %d"), i + 1));
            bHasSpawnedShareAOE = true;
        }
        else
        {
            SpawnSingleCircleAOE(RandomLocation, FString::Printf(TEXT("Random Circle AOE %d (Share Fallback)"), i + 1));
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("AOE Spawn Summary: %d Player-Target AOEs, %d Random AOEs, Share AOE: %s"),
        //PlayerAOECount, ExtraRandomAOEs, bHasSpawnedShareAOE ? TEXT("YES") : TEXT("NO"));
}

FVector ABossNPC::GetSafeRandomLocationAroundBoss()
{
    TArray<FVector> SafePositions = {
        FVector(-200.0f, 300.0f, 9.5f),
        FVector(-100.0f, 250.0f, 9.5f),
        FVector(0.0f, 400.0f, 9.5f),
        FVector(100.0f, 350.0f, 9.5f),
        FVector(200.0f, 200.0f, 9.5f),

        FVector(-240.0f, -600.0f, 9.5f),
        FVector(-150.0f, -800.0f, 9.5f),
        FVector(-50.0f, -700.0f, 9.5f),
        FVector(50.0f, -900.0f, 9.5f),
        FVector(150.0f, -750.0f, 9.5f),
        FVector(220.0f, -600.0f, 9.5f),

        FVector(-230.0f, -1800.0f, 9.5f),
        FVector(-180.0f, -2000.0f, 9.5f),
        FVector(-120.0f, -1600.0f, 9.5f),
        FVector(-60.0f, -2200.0f, 9.5f),
        FVector(0.0f, -1900.0f, 9.5f),
        FVector(60.0f, -2400.0f, 9.5f),
        FVector(120.0f, -1700.0f, 9.5f),
        FVector(180.0f, -2100.0f, 9.5f),
        FVector(210.0f, -2300.0f, 9.5f),

        FVector(-200.0f, -2900.0f, 9.5f),
        FVector(-120.0f, -3100.0f, 9.5f),
        FVector(-40.0f, -3000.0f, 9.5f),
        FVector(40.0f, -3200.0f, 9.5f),
        FVector(120.0f, -2800.0f, 9.5f),
        FVector(200.0f, -3050.0f, 9.5f),

        FVector(-180.0f, -3650.0f, 9.5f),
        FVector(-90.0f, -3700.0f, 9.5f),
        FVector(0.0f, -3600.0f, 9.5f),
        FVector(90.0f, -3680.0f, 9.5f),
        FVector(180.0f, -3620.0f, 9.5f)
    };

    int32 RandomIndex = FMath::RandRange(0, SafePositions.Num() - 1);
    FVector SelectedPosition = SafePositions[RandomIndex];

    //UE_LOG(LogTemp, Warning, TEXT("Selected safe random position: %s (Index: %d)"),
        //*SelectedPosition.ToString(), RandomIndex);

    return SelectedPosition;
}

void ABossNPC::SpawnSingleLargeCircleAOE(FVector SpawnLocation, FString AOEType)
{
    ServerSpawnSingleLargeCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::ServerSpawnSingleLargeCircleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    MultiSpawnSingleLargeCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::MultiSpawnSingleLargeCircleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
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
        SpawnedAOE->SetupAsLargeCircleAOE(800.0f);
        SpawnedAOE->StartAOE();
        //UE_LOG(LogTemp, Warning, TEXT("%s spawned at location: %s - LARGE CIRCLE AOE (Radius: 800)"),
            //*AOEType, *SpawnLocation.ToString());
    }
}

void ABossNPC::MultiSpawnAOEAttack_Implementation()
{

}

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
        //UE_LOG(LogTemp, Warning, TEXT("%s spawned at location: %s"),
            //*AOEType, *SpawnLocation.ToString());
    }
}

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
        SpawnedAOE->SetupAsShareAOE(300.0f, 2);
        SpawnedAOE->StartAOE();
        //UE_LOG(LogTemp, Warning, TEXT("%s spawned at location: %s - SHARE MECHANICS! (Need 2+ players)"),
            //*AOEType, *SpawnLocation.ToString());
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
           // UE_LOG(LogTemp, Log, TEXT("Player %s is nearby (Distance: %f)"), *Player->GetName(), Distance);
        }
    }

    return NearbyPlayers;
}

void ABossNPC::SpawnSingleAOE(FVector SpawnLocation, FString AOEType)
{
    SpawnSingleCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::ServerSpawnSingleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    ServerSpawnSingleCircleAOE(SpawnLocation, AOEType);
}

void ABossNPC::MultiSpawnSingleAOE_Implementation(FVector SpawnLocation, const FString& AOEType)
{
    MultiSpawnSingleCircleAOE(SpawnLocation, AOEType);
}

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

        ABossObstacle* NewObstacle = GetWorld()->SpawnActor<ABossObstacle>(
            ABossObstacle::StaticClass(),
            SpawnLocation,
            SpawnRotation,
            Params
        );
    }

    BMSoundPlay("Phase2");
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
    TArray<AActor*> FoundObstacles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossObstacle::StaticClass(), FoundObstacles);

    for (AActor* Obstacle : FoundObstacles)
    {
        if (Obstacle)
        {
            Obstacle->Destroy();
        }
    }
}

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

FVector ABossNPC::GetRandomLocationAroundBoss()
{
    FVector BossLocation = GetActorLocation();

    float MinSafeDistance = 600.0f;
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
        //UE_LOG(LogTemp, Warning, TEXT("AOE Pattern Stopped"));
    }
}

// 장애물 패턴 정지
void ABossNPC::StopObstaclePattern()
{
    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().ClearTimer(ObstaclePatternTimerHandle);
        //UE_LOG(LogTemp, Warning, TEXT("Obstacle Pattern Stopped"));
    }
}

// 네트워크 복제 설정
void ABossNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //DOREPLIFETIME(ABossNPC, CurrentPhase);
}