// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include "BossObstacle.generated.h"

UCLASS()
class LOSTSOMETHING_API ABossObstacle : public AActor
{
	GENERATED_BODY()

public:	
	ABossObstacle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 메시 (시각적 표현용)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ObstacleMesh;

	// 콜리전
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp;

	// 이동 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 600.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector MoveDirection;

};
