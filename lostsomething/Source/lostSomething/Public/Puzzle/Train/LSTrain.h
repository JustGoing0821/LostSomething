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
	float CurrentAlpha = 0.0f;
	float LerpSpeed = 0.5f;

	UPROPERTY(Replicated)
	FVector LeaveLocation;

	UPROPERTY(Replicated)
	FVector WaitLocation;

	UPROPERTY(Replicated)
	ETrainState CurrentTrainState;

	FVector ServerTrainMove;

//Gates
protected:
	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UStaticMeshComponent>> DoorLs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UStaticMeshComponent>> DoorRs;

	UPROPERTY(VisibleAnywhere, Category = Gate, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentOpenGate)
	int32 CurrentOpenGate;

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GateOpen();

	void GateClose();

public:
	UFUNCTION()
	void OnRep_CurrentOpenGate();

};
