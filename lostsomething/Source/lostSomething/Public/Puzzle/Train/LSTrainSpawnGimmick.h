// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSTrainSpawnGimmick.generated.h"

UENUM(BlueprintType)
enum class ETrainSpawnState : uint8
{
	Spawned = 0,
	Despawned
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPuzzleCheckDelegate, bool/*PuzzleCorrect*/, int32/*CorrectGate*/);

UCLASS()
class LOSTSOMETHING_API ALSTrainSpawnGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSTrainSpawnGimmick();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//Trigger Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> StageTrigger;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UBoxComponent>> WaitTriggers;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ALSTrain> TrainClass;

	TMap<int32, int32> CurrentOverlapTrigger;

	UFUNCTION()
	void OnSpawnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSpawnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnGateWaitTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateWaitTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


// Train Spawn Section
public:
	UPROPERTY(Replicated)
	ETrainSpawnState CurrentState;

protected:
	void SpawnTrain();


//Puzzle Section
public:
	FORCEINLINE void SetCorrectGate(int32 InCorrectOpenGate) { CorrectGate = InCorrectOpenGate; }

	FOnPuzzleCheckDelegate OnPuzzleCheck;

protected:
	int32 CorrectGate;

	void CheckPuzzleCorrect();
};
