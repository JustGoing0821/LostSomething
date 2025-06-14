// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TimerHandle.h"
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

    // 컴포넌트들
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* WarningMesh;

    // AOE 설정
    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Radius = 500.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float WarningDuration = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AOE Settings")
    float Damage = 20.0f;


    // 타이머
    FTimerHandle WarningTimerHandle;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // AOE 시작
    UFUNCTION(BlueprintCallable)
    void StartAOE();

    // 폭발
    UFUNCTION()
    void Explode();

private:
    void DealDamageToPlayersInRange();
};