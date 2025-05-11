// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSTrain.generated.h"

UENUM(BlueprintType)
enum class ETrainState : uint8
{
	Comming = 0,
	Waiting,
	Stop,
	Leaving
};

DECLARE_MULTICAST_DELEGATE(FOnTrainArrivedDelegate);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSTrain : public AActor
{
	GENERATED_BODY()
	
public:
	ALSTrain();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = Train, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> TrainTrigger;


//Train Move
public:
	UPROPERTY(Replicated)
	ETrainState CurrentTrainState;

	FOnTrainArrivedDelegate OnTrainArrived;

protected:
	float CurrentAlpha = 0.0f;
	float LerpSpeed = 0.5f;
	FVector WaitLocation;
	FVector LeaveLocation;


//Gates
public:
	FORCEINLINE void SetCorrectGate(int32 InCorrectOpenGate) { CorrectGate = InCorrectOpenGate; }

protected:
	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> DoorLs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> DoorRs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentOpenGate)
	int32 CurrentOpenGate;
	
	int32 CorrectGate;

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GateOpen();

	void GateClose();

//Puzzle
public:
	void DelegateBind(class ALSTrainSpawnGimmick* InGimmickClass);

protected:
	void PuzzleCheck(bool bCorrect, int32 InCorrectGate);

//Network
public:
	UFUNCTION()
	void OnRep_CurrentOpenGate();

//Replicate
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCGateOpen();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCGateClose();

};
