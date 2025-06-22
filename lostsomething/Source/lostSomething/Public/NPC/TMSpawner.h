// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TMSpawner.generated.h"

UCLASS()
class LOSTSOMETHING_API ATMSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATMSpawner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Monster Type")
	bool bBoss;

	// 몬스터 클래스 설정용
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> MonsterClass;

	// 스폰 위치 오프셋들 (예: 주변에 퍼지게 하고 싶을 때)
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<FVector> SpawnOffsets;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
