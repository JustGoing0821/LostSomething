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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> SharedRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> LeftSideGatesRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> RightSideGatesRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> LeftSideCrowdRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> RightSideCrowdRoot;

	UPROPERTY(VisibleAnywhere, Category = Train, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> TrainTrigger;


//Train Move
public:
	UPROPERTY(Replicated)
	ETrainState CurrentTrainState;

	FOnTrainArrivedDelegate OnTrainArrived;

protected:
	float CurrentTrainAlpha = 0.0f;
	float CurrentPassengersAlpha = 0.0f;
	float LerpSpeed = 0.5f;
	uint8 bisPassengersGettingOff : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	FVector WaitLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	FVector LeaveLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	FVector LeftSideCrowdGetOnLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	FVector RightSideCrowdGetOnLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	FVector LeftSideCrowdGetOffLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	FVector RightSideCrowdGetOffLocation;

//Gates
public:
	FORCEINLINE void SetCorrectGate(int32 InCorrectOpenGate) { CorrectGate = InCorrectOpenGate; }

	UPROPERTY(Replicated)
	int32 CorrectDoorIndex;

protected:
	UPROPERTY(EditAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	bool bisGateLeftSide;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> LeftSideDoorLs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> LeftSideDoorRs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> RightSideDoorLs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> RightSideDoorRs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> LeftSideCrowds;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> RightSideCrowds;
	
	int32 CorrectGate;

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GateOpen();

	void GateClose();

//Puzzle
public:
	void PuzzleCheck(bool bCorrect, int32 InCorrectGate);
	void StopTrain();

protected:
	void GetOffPassengers(int32 InCorrectGate);
	void GetOnPassengers();


	//Timer
	FTimerHandle TrainTimerHandle;

	UPROPERTY(EditAnywhere, Category="Puzzle")
	float TimeBeforeGateOpen;

	UPROPERTY(EditAnywhere, Category = "Puzzle", Replicated)
	float TimeTrainWait;

	UPROPERTY(EditAnywhere, Category = "Puzzle")
	float TimeBeforeTrainLeave;


//Replicate
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCGateOpen();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCGateClose();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastGetOffPassengers(int32 InCorrectGate);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastGetOnPassengers();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStopTrain();

};
