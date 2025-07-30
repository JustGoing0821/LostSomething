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

UCLASS()
class LOSTSOMETHING_API ACircleAOE : public AActor
{
    GENERATED_BODY()

    public:
    // Sets default values for this actor's properties
    ACircleAOE();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // 리플리케이션 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 컴포넌트들
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* WarningMesh;

    // AOE 설정
    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Radius = 250.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float WarningDuration = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Damage = 20.0f;

    // 머티리얼 설정
    UPROPERTY(EditAnywhere, Category = "Visual")
    class UMaterialInterface* WarningMaterial;

    // 애니메이션용 - 복제되는 변수들
    UPROPERTY(Replicated)
    float ElapsedTime = 0.0f;

    UPROPERTY(ReplicatedUsing = OnRep_IsWarningPhase)
    bool bIsWarningPhase = false;

    UMaterialInstanceDynamic* DynamicMaterial = nullptr;

    // 타이머
    FTimerHandle WarningTimerHandle;

    // RepNotify 함수
    UFUNCTION()
    void OnRep_IsWarningPhase();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // AOE 시작
    UFUNCTION(BlueprintCallable)
    void StartAOE();

    // 폭발
    UFUNCTION()
    void Explode();

    // RPC 함수들
    UFUNCTION(NetMulticast, Reliable)
    void MulticastStartWarning();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastExplode();

private:
    void DealDamageToPlayersInRange();
    void UpdateColorAnimation(float Alpha);
    void SetupWarningVisual();
};