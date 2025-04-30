// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSTrain.generated.h"

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

	//Train Move
public:
	UPROPERTY(ReplicatedUsing = OnRep_ServerTrainMove)
	FVector ServerTrainMove;

	UFUNCTION()
	void OnRep_ServerTrainMove();
};
