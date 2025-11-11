// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Engine/TimerHandle.h"
#include "Net/UnrealNetwork.h"
#include "ACircleAOE.generated.h"

// Forward declarations for Niagara
class UNiagaraComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EAOEType : uint8
{
    Circle      UMETA(DisplayName = "Circle"),
    Share       UMETA(DisplayName = "Share")
};

UCLASS()
class LOSTSOMETHING_API ACircleAOE : public AActor
{
    GENERATED_BODY()

    public:
    ACircleAOE();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Components
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    // 나이아가라 이펙트 컴포넌트들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    UNiagaraComponent* WarningEffect;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    UNiagaraComponent* ExplosionEffect;

    // 나이아가라 시스템 에셋들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* CircleWarningEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* CircleExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* ShareWarningEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* ShareExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* LargeCircleWarningEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* LargeCircleExplosionEffect;

    // 각 AOE 타입별 색상 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX|Colors")
    FLinearColor CircleWarningColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // 주황색

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX|Colors")
    FLinearColor CircleExplosionColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX|Colors")
    FLinearColor ShareWarningColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // 청록색

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX|Colors")
    FLinearColor ShareExplosionColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // 파란색

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX|Colors")
    FLinearColor LargeCircleWarningColor = FLinearColor(1.0f, 0.0f, 1.0f, 1.0f); // 마젠타

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX|Colors")
    FLinearColor LargeCircleExplosionColor = FLinearColor(0.5f, 0.0f, 1.0f, 1.0f); // 보라색

    // AOE type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOE Type")
    EAOEType AOEType = EAOEType::Circle;

    // Large Circle 구분용
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "AOE Type")
    bool bIsLargeCircle = false;

    // Base AOE
    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Radius = 250.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float WarningDuration = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Damage = 20.0f;

    // Explosion effect duration
    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float ExplosionEffectDuration = 0.5f;

    // Share settings
    UPROPERTY(EditAnywhere, Category = "Share Settings")
    float ShareWarningDuration = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Share Settings", meta = (EditCondition = "AOEType == EAOEType::Share"))
    int32 MinSharePlayers = 2;

    UPROPERTY(EditAnywhere, Category = "Share Settings", meta = (EditCondition = "AOEType == EAOEType::Share"))
    float ShareDamageReduction = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Share Settings", meta = (EditCondition = "AOEType == EAOEType::Share"))
    float MaxSoloFatalDamage = 1000.0f;

    UPROPERTY(Replicated)
    AActor* TrackedPlayer = nullptr;

    UPROPERTY(EditAnywhere, Category = "Share Settings")
    bool bShouldTrackPlayer = true;

    UPROPERTY(EditAnywhere, Category = "Share Settings")
    float TrackingSpeed = 500.0f;

    // Replicated anim vars
    UPROPERTY(Replicated)
    float ElapsedTime = 0.0f;

    UPROPERTY(ReplicatedUsing = OnRep_IsWarningPhase)
    bool bIsWarningPhase = false;

    // Timers
    FTimerHandle WarningTimerHandle;
    FTimerHandle TrackingTimerHandle;
    FTimerHandle ExplosionEffectTimerHandle;
    FTimerHandle DestroyTimerHandle;

    UFUNCTION()
    void OnRep_IsWarningPhase();

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void StartAOE();

    UFUNCTION()
    void Explode();

    UFUNCTION()
    void DestroyAOE();

    // Setup helpers
    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsCircleAOE(float InRadius);

    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsShareAOE(float InRadius, int32 InMinPlayers = 2);

    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsLargeCircleAOE(float InRadius);

    // Tracking
    UFUNCTION(BlueprintCallable)
    void SetTrackedPlayer(AActor* Player);

    UFUNCTION()
    void UpdateTrackingPosition();

    UFUNCTION()
    void ApplyOutOfRangePenalty();

    // RPCs
    UFUNCTION(NetMulticast, Reliable)
    void MulticastStartWarning();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastExplode();

private:
    void DealDamageToPlayersInRange();
    void UpdateColorAnimation(float Alpha);
    void SetupWarningVisual();

    bool IsPlayerInCircle(AActor* Player);
    bool IsPlayerInShare(AActor* Player);

    float CalculateCircleDamage(AActor* Player);
    float CalculateShareDamage(AActor* Player, const TArray<AActor*>& PlayersInRange);

    void SetupCircleVisual();
    void SetupShareVisual();
    void SetupLargeCircleVisual();

    void UpdateCircleAnimation(float Alpha);
    void UpdateShareAnimation(float Alpha);
    void UpdateLargeCircleAnimation(float Alpha);

    void StartPlayerTracking();
    void StopPlayerTracking();

    TArray<AActor*> GetPlayersInRange();
    float GetAngleBetweenVectors(FVector A, FVector B);
};