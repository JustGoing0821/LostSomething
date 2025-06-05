// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/LSTakeDamageInterface.h"
#include "BossNPC.generated.h"

UCLASS()
class LOSTSOMETHING_API ABossNPC : public ACharacter, public ILSTakeDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossNPC();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle")
	TArray<USceneComponent*> ObstacleSpawnPoints;

	FORCEINLINE void SetHP(float NewHP)
	{
		CurrentHP = (NewHP < 0.0f) ? 0.0f : NewHP;
	}
	FORCEINLINE float GetHP() const { return CurrentHP; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float MaxHP = 100.0f;
	float CurrentHP;

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

	void EnterPhase3();
	void SpawnPlatform();

	UFUNCTION(Server, Reliable)
	void ServerSpawnPlatform();

	UPROPERTY()
	class APlatformGenerator* PlatformGenerator;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlatformGenerator> PlatformGeneratorClass;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);


};
