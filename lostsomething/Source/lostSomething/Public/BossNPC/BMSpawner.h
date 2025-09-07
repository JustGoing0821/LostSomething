// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BMSpawner.generated.h"

UCLASS()
class LOSTSOMETHING_API ABMSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABMSpawner();

	UFUNCTION()
	void SpawnBoss();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
