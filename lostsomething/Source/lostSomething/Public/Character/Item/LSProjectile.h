// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Interface/LSTakeDamageInterface.h"
#include "LSProjectile.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //구체 콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    USphereComponent* CollisionComponent;

    //메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UStaticMeshComponent* ProjectileMesh;

    //무브먼트 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;




    // 속성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float InitialSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float MaxLifetime;

    //on hit 함수
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    
    
    void FireInDirection(const FVector& Direction);
};
