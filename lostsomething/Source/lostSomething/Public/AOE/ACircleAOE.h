// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TimerHandle.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "ACircleAOE.generated.h"

UENUM(BlueprintType)
enum class EAOEType : uint8
{
    Circle      UMETA(DisplayName = "Circle"),
    Cone        UMETA(DisplayName = "Cone"),
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

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* WarningMesh;

    // AOE type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOE Type")
    EAOEType AOEType = EAOEType::Circle;

    // Base AOE
    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Radius = 250.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float WarningDuration = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Damage = 20.0f;

    // Cone settings
    UPROPERTY(EditAnywhere, Category = "Cone Settings", meta = (EditCondition = "AOEType == EAOEType::Cone"))
    float ConeAngle = 90.0f;

    UPROPERTY(EditAnywhere, Category = "Cone Settings", meta = (EditCondition = "AOEType == EAOEType::Cone"))
    float ConeRange = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Cone Settings", meta = (EditCondition = "AOEType == EAOEType::Cone"))
    FVector ConeDirection = FVector(1.0f, 0.0f, 0.0f);

    // Share settings
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

    // Visuals
    UPROPERTY(EditAnywhere, Category = "Visual")
    class UMaterialInterface* WarningMaterial;

    UPROPERTY(EditAnywhere, Category = "Visual")
    class UMaterialInterface* ShareWarningMaterial;

    UPROPERTY(EditAnywhere, Category = "Visual")
    class UMaterialInterface* ConeWarningMaterial;

    // Replicated anim vars
    UPROPERTY(Replicated)
    float ElapsedTime = 0.0f;

    UPROPERTY(ReplicatedUsing = OnRep_IsWarningPhase)
    bool bIsWarningPhase = false;

    UMaterialInstanceDynamic* DynamicMaterial = nullptr;

    // Timers
    FTimerHandle WarningTimerHandle;
    FTimerHandle TrackingTimerHandle;

    UFUNCTION()
    void OnRep_IsWarningPhase();

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void StartAOE();

    UFUNCTION()
    void Explode();

    // Setup helpers
    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsCircleAOE(float InRadius);

    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsConeAOE(float InRange, float InAngle, FVector InDirection);

    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsShareAOE(float InRadius, int32 InMinPlayers = 2);

    UFUNCTION(BlueprintCallable, Category = "AOE Setup")
    void SetupAsConeFromBoss(AActor* BossActor, float InRange, float InAngle);

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
    bool IsPlayerInCone(AActor* Player);
    bool IsPlayerInShare(AActor* Player);

    float CalculateCircleDamage(AActor* Player);
    float CalculateConeDamage(AActor* Player);
    float CalculateShareDamage(AActor* Player, const TArray<AActor*>& PlayersInRange);

    void SetupCircleVisual();
    void SetupConeVisual();
    void SetupShareVisual();

    void UpdateCircleAnimation(float Alpha);
    void UpdateConeAnimation(float Alpha);
    void UpdateShareAnimation(float Alpha);

    void StartPlayerTracking();
    void StopPlayerTracking();

    TArray<AActor*> GetPlayersInRange();
    float GetAngleBetweenVectors(FVector A, FVector B);
};
