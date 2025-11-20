// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/CapsuleComponent.h>
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
	UCapsuleComponent* CollisionComp;

	// 이동 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 600.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector MoveDirection;

	const float AttackDamage = 50.0f;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

};
