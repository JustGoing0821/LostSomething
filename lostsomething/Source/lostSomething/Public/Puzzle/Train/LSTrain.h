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
	float LerpSpeed = 2.0f;

	UPROPERTY(Replicated)
	FVector LeaveLocation;

	UPROPERTY(Replicated)
	FVector WaitLocation;

	UPROPERTY(Replicated)
	ETrainState CurrentTrainState;

	//UPROPERTY(ReplicatedUsing = OnRep_ServerTrainMove)
	FVector ServerTrainMove;

	//UFUNCTION()
	//void OnRep_ServerTrainMove();
};
