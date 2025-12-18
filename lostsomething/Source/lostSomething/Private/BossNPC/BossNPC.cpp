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
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include <BossNPC/Obstacle/SpecialObstacle.h>

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

    for (int i = 0; i < 9; ++i)
    {
        FString Name = FString::Printf(TEXT("SpawnPoint_%d"), i);
        USceneComponent* SpawnPoint = CreateDefaultSubobject<USceneComponent>(*Name);
        SpawnPoint->SetupAttachment(RootComponent);
        float YOffset = (i - 3) * 76.0f;
        SpawnPoint->SetRelativeLocation(FVector(350.f, YOffset, -20.0f));
        ObstacleSpawnPoints.Add(SpawnPoint);
    }

    CurrentHP = MaxHP;

    // AOE 관련 기본값 설정
    AOEAttackInterval = 15.0f;
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
    UGameplayStatics::PlaySound2D(GetWorld(), SelectedSound, 0.5f);
}

// Called when the game starts or when spawned
void ABossNPC::BeginPlay()
{
    Super::BeginPlay();
    // Dynamic Material Instance 생성
    if (GetMesh() && GetMesh()->GetMaterial(0))
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
        GetMesh()->SetMaterial(0, DynamicMaterial);
    }
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
    ServerBMFlashDamageColor();
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

    if (CurrentPhase == 1 && NewHP <= 160.0f)
    {
        NewHP = 160.0f;
        CurrentPhase = 2;
        bIsPhaseChanging = true;
        DamageMontagePlay();
        HideGroupActor(2);
    }
    else if (CurrentPhase == 2 && NewHP <= 80.0f)
    {
        DestroyObstacles();
        NewHP = 80.0f;
        CurrentPhase = 3; 
        bIsPhaseChanging = true;
        DamageMontagePlay();
        HideGroupActor(3);
    }
    CurrentHP = (NewHP < 0.0f) ? 0.0f : NewHP;
}

void ABossNPC::ServerBMFlashDamageColor_Implementation()
{
    MultiBMFlashDamageColor();
}

void ABossNPC::MultiBMFlashDamageColor_Implementation()
{
    if (DynamicMaterial)
    {
        // 효과 시작 시간 기록
        BMFlashStartTime = GetWorld()->GetRealTimeSeconds();

        // 기존에 실행 중인 타이머가 있다면 중복 실행을 막기 위해 초기화
        GetWorld()->GetTimerManager().ClearTimer(BMFlashUpdateTimerHandle);

        // 타이머 설정: 0.016초마다 UpdateFlashEffect_Timer 함수를 반복 호출
        GetWorld()->GetTimerManager().SetTimer(
            BMFlashUpdateTimerHandle,      // 타이머 핸들
            this,                        // 함수를 호출할 오브젝트
            &ABossNPC::BMUpdateFlashEffect_Timer, // 호출할 함수
            0.016f,                      // 호출 간격 (초)
            true,                        // 반복 여부
            0.0f                         // 즉시 시작
        );
    }
}

void ABossNPC::BMUpdateFlashEffect_Timer()
{
    ServerBMUpdateFlashEffect_Timer();
}

void ABossNPC::ServerBMUpdateFlashEffect_Timer_Implementation()
{
    MultiBMUpdateFlashEffect_Timer();
}

void ABossNPC::MultiBMUpdateFlashEffect_Timer_Implementation()
{
    if (DynamicMaterial)
    {
        const float ElapsedTime = GetWorld()->GetRealTimeSeconds() - BMFlashStartTime;

        if (ElapsedTime < BMDamageFlashDuration)
        {
            // 경과 시간에 따라 Alpha 값을 1.0에서 0.0으로 부드럽게 보간
            const float CurrentAlpha = FMath::Lerp(0.2f, 0.0f, ElapsedTime / BMDamageFlashDuration);
            DynamicMaterial->SetScalarParameterValue(FName("Alpha"), CurrentAlpha);
        }
        else
        {
            // 시간이 다 되면 효과를 확실하게 끝내고 타이머를 정지
            DynamicMaterial->SetScalarParameterValue(FName("Alpha"), 0.0f);
            GetWorld()->GetTimerManager().ClearTimer(BMFlashUpdateTimerHandle);
        }
    }
}


// 1페이즈 진입 - AOE 패턴
void ABossNPC::EnterPhase1()
{
    if (HasAuthority())
    {
        this->SetActorLocationAndRotation(
            FVector(-4.363589f, -3764.906206f, 73.247542f),
            FRotator(0.0f, 90.0f, 0.0f));
        StartAOEAttackPattern();
    }
}

// 2페이즈 진입 - 장애물 패턴
void ABossNPC::EnterPhase2()
{
    if (HasAuthority())
    {
        this->SetActorLocationAndRotation(
            FVector(-4.363589f, -3764.906206f, 73.247542f),
            FRotator(0.0f, 90.0f, 0.0f));
        // 장애물 스폰 패턴 시작
        SpawnObstacles();
    }
}

// 3페이즈 진입 - 플랫폼 생성
void ABossNPC::EnterPhase3()
{
    if (HasAuthority())
    {
        this->SetActorLocationAndRotation(
            FVector(-4.363589f, -3764.906206f, 73.247542f),
            FRotator(0.0f, 90.0f, 0.0f));
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

void ABossNPC::HideGroupActor(int32 InCurrentPhase)
{
    if (HasAuthority())
    {
        MultiHideGroupActor(InCurrentPhase);
    }
}

//void ABossNPC::ServerHideGroupActor_Implementation(int32 CurrentPhase)
//{
//    MultiHideGroupActor(CurrentPhase);
//}

void ABossNPC::MultiHideGroupActor_Implementation(int32 InCurrentPhase)
{
    if (InCurrentPhase == 2)
    {
        //LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Phase2Remove Begin"));
        TArray<AActor*> Phase2RemoveActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Phase2Remove")), Phase2RemoveActors);
        for (AActor* Actor : Phase2RemoveActors)
        {
            if (Actor)
            {
                Actor->SetActorHiddenInGame(true);
                Actor->SetActorEnableCollision(false);
            }
        }
    }
    else
    {
        //LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Phase3Remove Begin"));
        TArray<AActor*> Phase2RemoveActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Phase2Remove")), Phase2RemoveActors);
        for (AActor* Actor : Phase2RemoveActors)
        {
            if (Actor)
            {
                Actor->SetActorHiddenInGame(true);
                Actor->SetActorEnableCollision(false);
            }
        }

        TArray<AActor*> Phase3RemoveActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Phase3Remove")), Phase3RemoveActors);
        for (AActor* Actor : Phase3RemoveActors)
        {
            if (Actor)
            {
                Actor->SetActorHiddenInGame(true);
                Actor->SetActorEnableCollision(false);
            }
        }
    }

    //for (AActor* Actor : FoundActors)
    //{
    //    if (Actor)
    //    {
    //        Actor->SetActorHiddenInGame(true);
    //        Actor->SetActorEnableCollision(false);
    //    }
    //}
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
    ILSQuestInterface* GM = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
    {
        GM->QuestComplete();
    }

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

    /* if (bShouldSpawnLargeAOE)
    {
        FVector FixedLargeAOELocation = FVector(-33.409138f, -4088.049805f, 9.5f);

        SpawnSingleLargeCircleAOE(FixedLargeAOELocation, FString::Printf(TEXT("Fixed Position Large Circle AOE")));
        return; 
    }
    */

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

    // 특수 장애물을 스폰할 인덱스 선택
    TArray<int32> SpecialIndexes;
    for (int32 i = 0; i < SpawnPointCount; ++i)
    {
        SpecialIndexes.Add(i);
    }

    // 랜덤 셔플
    for (int32 i = 0; i < SpecialIndexes.Num(); ++i)
    {
        int32 RandIdx = FMath::RandRange(i, SpecialIndexes.Num() - 1);
        SpecialIndexes.Swap(i, RandIdx);
    }

    // 3~4개를 특수 장애물로 지정
    int32 NumSpecial = FMath::RandRange(2, 3);
    TSet<int32> SpecialSet;
    for (int32 i = 0; i < NumSpecial; ++i)
    {
        SpecialSet.Add(SpecialIndexes[i]);
    }

    // 모든 스폰 포인트에 장애물 생성
    for (int32 i = 0; i < SpawnPointCount; ++i)
    {
        USceneComponent* SpawnPoint = ObstacleSpawnPoints[i];
        if (!SpawnPoint) continue;

        FVector SpawnLocation = SpawnPoint->GetComponentLocation();
        FRotator SpawnRotation = GetActorRotation();
        FActorSpawnParameters Params;
        Params.Owner = this;

        // 특수 인덱스면 특수 장애물, 아니면 일반 장애물
        if (SpecialSet.Contains(i))
        {
            // 특수 장애물 스폰
            ASpecialObstacle* NewObstacle = GetWorld()->SpawnActor<ASpecialObstacle>(
               ASpecialObstacle::StaticClass(),
                SpawnLocation,
                SpawnRotation,
                Params
            );
        }
        else
        {
            // 일반 장애물 스폰
            ABossObstacle* NewObstacle = GetWorld()->SpawnActor<ABossObstacle>(
                ABossObstacle::StaticClass(),
                SpawnLocation,
                SpawnRotation,
                Params
            );
        }
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

    TArray<AActor*> FoundSObstacles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpecialObstacle::StaticClass(), FoundObstacles);

    for (AActor* SObstacle : FoundObstacles)
    {
        if (SObstacle)
        {
            SObstacle->Destroy();
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