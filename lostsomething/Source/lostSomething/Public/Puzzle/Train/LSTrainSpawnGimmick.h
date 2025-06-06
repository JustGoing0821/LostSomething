// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSTrainSpawnGimmick.generated.h"

UENUM(BlueprintType)
enum class ETrainSpawnState : uint8
{
	Spawned = 0,
	Despawned
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTrainPuzzleCheckDelegate, bool/*PuzzleCorrect*/, int32/*CorrectGate*/);
DECLARE_DELEGATE(FOnTrainDespawnedDelegate);

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

//Stage Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> StageTrigger;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ALSTrain> TrainClass;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ALSTrainStep> StepTriggerClass;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TArray<class UBoxComponent*> WaitTriggers;

	TMap<int32, int32> CurrentOverlapTrigger;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TArray<FVector> StepTriggerLocations;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> PannelMesh;

	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TArray<class UMaterialInterface*> PannelMaterials;


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

	FOnTrainDespawnedDelegate OnTrainDespawned;

protected:
	void SpawnTrain();


//Puzzle Section
public:
	FORCEINLINE void SetCorrectGate(int32 InCorrectOpenGate) { CorrectGate = InCorrectOpenGate; }

	FOnTrainPuzzleCheckDelegate OnPuzzleCheck;

	UPROPERTY(ReplicatedUsing = OnRep_CorrectGate)
	int32 CorrectGate;

protected:
	void CheckPuzzleCorrect();
	void SetPannelMonitor();
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();

	int32 CorrectPeopleCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;


//RPC Section
public:
	UFUNCTION()
	void OnRep_CorrectGate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSetPannelMonitor();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();
};
