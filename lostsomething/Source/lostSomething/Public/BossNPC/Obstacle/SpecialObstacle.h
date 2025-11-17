// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/CapsuleComponent.h>
#include "SpecialObstacle.generated.h"

UCLASS()
class LOSTSOMETHING_API ASpecialObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpecialObstacle();

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

	void SO_ChangeVisible();

	void SO_SetVisibleSiJae();
	void SO_SetVisibleIJae();

	UPROPERTY()
	UMaterialInterface* Material_IJae;

	UPROPERTY()
	UMaterialInterface* Material1_SiJae;
	UPROPERTY()
	UMaterialInterface* Material2_SiJae;
	UPROPERTY()
	UMaterialInterface* Material3_SiJae;

	FVector MoveDirection;

	const float AttackDamage = 30.0f;

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
