// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossNPC.generated.h"

UCLASS()
class LOSTSOMETHING_API ABossNPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossNPC();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle")
	TArray<USceneComponent*> ObstacleSpawnPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SpawnObstacles();

	UFUNCTION(Server, Reliable)
	void ServerSpawnObstacles();
	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnObstacles();

};
