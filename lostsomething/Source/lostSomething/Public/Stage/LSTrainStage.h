// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSTrainStage.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSTrainStage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSTrainStage();
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
