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
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

ACircleAOE::ACircleAOE()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(RootComponent);
    CollisionSphere->SetSphereRadius(Radius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    WarningEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WarningEffect"));
    WarningEffect->SetupAttachment(RootComponent);
    WarningEffect->SetAutoActivate(false);

    ExplosionEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionEffect"));
    ExplosionEffect->SetupAttachment(RootComponent);
    ExplosionEffect->SetAutoActivate(false);

    bShouldTrackPlayer = true;
    TrackingSpeed = 500.0f;
    bIsLargeCircle = false;
    ExplosionEffectDuration = 0.5f;
}

void ACircleAOE::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACircleAOE, ElapsedTime);
    DOREPLIFETIME(ACircleAOE, bIsWarningPhase);
    DOREPLIFETIME(ACircleAOE, TrackedPlayer);
    DOREPLIFETIME(ACircleAOE, bIsLargeCircle);
}

void ACircleAOE::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }
}

void ACircleAOE::SetupAsCircleAOE(float InRadius)
{
    UE_LOG(LogTemp, Warning, TEXT("SetupAsCircleAOE called! Radius: %f"), InRadius);
    
    AOEType = EAOEType::Circle;
    Radius = InRadius;
    bIsLargeCircle = false;
    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("About to call SetupWarningVisual()"));
    SetupWarningVisual();
    UE_LOG(LogTemp, Warning, TEXT("SetupWarningVisual() completed"));
}

void ACircleAOE::SetupAsShareAOE(float InRadius, int32 InMinPlayers)
{
    AOEType = EAOEType::Share;
    Radius = InRadius;
    MinSharePlayers = InMinPlayers;
    bShouldTrackPlayer = true;
    bIsLargeCircle = false;

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
    }

    if (HasAuthority())
    {
        TArray<AActor*> AllPlayers;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSPlayer::StaticClass(), AllPlayers);

        if (AllPlayers.Num() > 0)
        {
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

void ACircleAOE::SetupAsLargeCircleAOE(float InRadius)
{
    AOEType = EAOEType::Circle;
    Radius = InRadius;
    bIsLargeCircle = true;
    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(Radius);
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

    GetWorld()->GetTimerManager().SetTimer(
        TrackingTimerHandle,
        this,
        &ACircleAOE::UpdateTrackingPosition,
        0.1f,
        true
    );

    UE_LOG(LogTemp, Warning, TEXT("Share AOE: Started tracking player %s"), *TrackedPlayer->GetName());
}

void ACircleAOE::ApplyOutOfRangePenalty()
{
    return;
}

void ACircleAOE::UpdateTrackingPosition()
{
    if (!HasAuthority() || !TrackedPlayer || !bShouldTrackPlayer) return;

    FVector TargetLocation = TrackedPlayer->GetActorLocation();
    TargetLocation.Z = 9.5f;

    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, 0.1f, TrackingSpeed);

    SetActorLocation(NewLocation);

    UE_LOG(LogTemp, Log, TEXT("Share AOE: Tracking player - moved to %s"), *NewLocation.ToString());
}

void ACircleAOE::StopPlayerTracking()
{
    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().ClearTimer(TrackingTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("Share AOE: Stopped tracking"));
    }
}

void ACircleAOE::SetupWarningVisual()
{
    switch (AOEType)
    {
    case EAOEType::Circle:
        if (bIsLargeCircle)
        {
            SetupLargeCircleVisual();
        }
        else
        {
            SetupCircleVisual();
        }
        break;
    case EAOEType::Share:
        SetupShareVisual();
        break;
    }
}

void ACircleAOE::SetupCircleVisual()
{
    if (CircleWarningEffect && WarningEffect)
    {
        WarningEffect->SetAsset(CircleWarningEffect);

        WarningEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
        WarningEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), CircleWarningColor);

        UE_LOG(LogTemp, Warning, TEXT("Circle Warning Effect configured: Scale 1.0"));
    }

    if (CircleExplosionEffect && ExplosionEffect)
    {
        ExplosionEffect->SetAsset(CircleExplosionEffect);

        ExplosionEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
        ExplosionEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), CircleExplosionColor);
    }
}

void ACircleAOE::SetupShareVisual()
{
    if (ShareWarningEffect && WarningEffect)
    {
        WarningEffect->SetAsset(ShareWarningEffect);

        WarningEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
        WarningEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), ShareWarningColor);
    }

    if (ShareExplosionEffect && ExplosionEffect)
    {
        ExplosionEffect->SetAsset(ShareExplosionEffect);

        ExplosionEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
        ExplosionEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), ShareExplosionColor);
    }
}

void ACircleAOE::SetupLargeCircleVisual()
{
    if (LargeCircleWarningEffect && WarningEffect)
    {
        WarningEffect->SetAsset(LargeCircleWarningEffect);

        WarningEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
        WarningEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), LargeCircleWarningColor);
    }

    if (LargeCircleExplosionEffect && ExplosionEffect)
    {
        ExplosionEffect->SetAsset(LargeCircleExplosionEffect);

        ExplosionEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
        ExplosionEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), LargeCircleExplosionColor);
    }
}

void ACircleAOE::OnRep_IsWarningPhase()
{
    if (bIsWarningPhase)
    {
        if (WarningEffect)
        {
            WarningEffect->Activate(true);
        }
    }
    else
    {
        if (WarningEffect)
        {
            WarningEffect->Deactivate();
        }
    }
}

void ACircleAOE::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsWarningPhase)
    {
        if (HasAuthority())
        {
            ElapsedTime += DeltaTime;
        }

        float Alpha = ElapsedTime / WarningDuration;
        UpdateColorAnimation(Alpha);
    }
}

void ACircleAOE::StartAOE()
{
    if (!HasAuthority())
        return;

    bIsWarningPhase = true;
    ElapsedTime = 0.0f;

    if (AOEType == EAOEType::Share && bShouldTrackPlayer)
    {
        StartPlayerTracking();
    }

    MulticastStartWarning();

    float Duration = (AOEType == EAOEType::Share) ? ShareWarningDuration : WarningDuration;

    GetWorld()->GetTimerManager().SetTimer(
        WarningTimerHandle,
        this,
        &ACircleAOE::Explode,
        Duration,
        false
    );
}

void ACircleAOE::MulticastStartWarning_Implementation()
{
    if (WarningEffect)
    {
        WarningEffect->Activate(true);
        UE_LOG(LogTemp, Warning, TEXT("Warning Effect (Holy Light) Activated!"));
    }
}

void ACircleAOE::Explode()
{
    if (!HasAuthority())
        return;

    bIsWarningPhase = false;

    if (AOEType == EAOEType::Share)
    {
        StopPlayerTracking();
    }

    MulticastExplode();

    if (CollisionSphere)
    {
        CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }

    DealDamageToPlayersInRange();

    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimerHandle,
        this,
        &ACircleAOE::DestroyAOE,
        ExplosionEffectDuration + 0.1f,
        false
    );
}

void ACircleAOE::MulticastExplode_Implementation()
{
    if (WarningEffect)
    {
        WarningEffect->Deactivate();
    }

    if (ExplosionEffect)
    {
        ExplosionEffect->Activate(true);
        UE_LOG(LogTemp, Warning, TEXT("Explosion Effect (Light Beam) Activated!"));
        
        GetWorld()->GetTimerManager().SetTimer(
            ExplosionEffectTimerHandle,
            [this]()
            {
                if (ExplosionEffect)
                {
                    ExplosionEffect->Deactivate();
                }
            },
            ExplosionEffectDuration,
            false
        );
    }
}

void ACircleAOE::DestroyAOE()
{
    Destroy();
}

void ACircleAOE::UpdateColorAnimation(float Alpha)
{
    switch (AOEType)
    {
    case EAOEType::Circle:
        if (bIsLargeCircle)
        {
            UpdateLargeCircleAnimation(Alpha);
        }
        else
        {
            UpdateCircleAnimation(Alpha);
        }
        break;
    case EAOEType::Share:
        UpdateShareAnimation(Alpha);
        break;
    }
}

void ACircleAOE::UpdateCircleAnimation(float Alpha)
{
    if (WarningEffect)
    {
        FLinearColor CurrentColor = FMath::Lerp(
            CircleWarningColor,
            CircleExplosionColor,
            Alpha
        );

        WarningEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), CurrentColor);
        WarningEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
    }
}

void ACircleAOE::UpdateShareAnimation(float Alpha)
{
    if (WarningEffect)
    {
        FLinearColor CurrentColor = FMath::Lerp(
            ShareWarningColor,
            ShareExplosionColor,
            Alpha
        );

        WarningEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), CurrentColor);
        WarningEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
    }
}

void ACircleAOE::UpdateLargeCircleAnimation(float Alpha)
{
    if (WarningEffect)
    {
        FLinearColor CurrentColor = FMath::Lerp(
            LargeCircleWarningColor,
            LargeCircleExplosionColor,
            Alpha
        );

        WarningEffect->SetNiagaraVariableLinearColor(TEXT("User.MainColor"), CurrentColor);
        WarningEffect->SetNiagaraVariableFloat(TEXT("User.Scale"), 1.0f);
    }
}

void ACircleAOE::DealDamageToPlayersInRange()
{
    if (AOEType == EAOEType::Share)
    {
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
                default:
                    break;
                }

                if (FinalDamage > 0.0f)
                {
                    FDamageEvent DamageEvent(UDamageType::StaticClass());
                    LSPlayer->TakeDamage(FinalDamage, DamageEvent, nullptr, this);

                    UE_LOG(LogTemp, Warning, TEXT("%s AOE dealt %f damage to %s"),
                        bIsLargeCircle ? TEXT("Large Circle") : TEXT("Circle"), FinalDamage, *LSPlayer->GetName());
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

bool ACircleAOE::IsPlayerInShare(AActor* Player)
{
    return IsPlayerInCircle(Player);
}

float ACircleAOE::CalculateCircleDamage(AActor* Player)
{
    return IsPlayerInCircle(Player) ? Damage : 0.0f;
}

float ACircleAOE::CalculateShareDamage(AActor* Player, const TArray<AActor*>& PlayersInRange)
{
    if (!IsPlayerInShare(Player))
    {
        UE_LOG(LogTemp, Warning, TEXT("Share AOE EXPLODE: %s is OUT OF SHARE RANGE when exploded - FATAL DAMAGE!"), *Player->GetName());
        return MaxSoloFatalDamage;
    }

    int32 PlayersInShareRange = 0;
    for (AActor* CheckPlayer : PlayersInRange)
    {
        if (IsPlayerInShare(CheckPlayer))
            PlayersInShareRange++;
    }

    if (PlayersInShareRange < MinSharePlayers)
    {
        UE_LOG(LogTemp, Warning, TEXT("Share AOE EXPLODE: %s is in range but not enough total players! (%d/%d) - Fatal damage!"),
            *Player->GetName(), PlayersInShareRange, MinSharePlayers);
        return MaxSoloFatalDamage;
    }
    else
    {
        float SharedDamage = Damage / PlayersInShareRange * ShareDamageReduction;
        UE_LOG(LogTemp, Warning, TEXT("Share AOE EXPLODE: %s - Damage shared among %d players - %f damage each"),
            *Player->GetName(), PlayersInShareRange, SharedDamage);
        return SharedDamage;
    }
}

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