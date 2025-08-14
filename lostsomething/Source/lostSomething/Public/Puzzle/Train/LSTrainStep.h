// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/LSInteractionInterface.h"
#include "LSTrainStep.generated.h"

DECLARE_DELEGATE(FOnStepInstalledDelegate);

UCLASS()
class LOSTSOMETHING_API ALSTrainStep : public AActor, public ILSInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSTrainStep();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

//Mesh Section
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

//Interaction Section
public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> InteractionTrigger;


//StepInstall
protected:
	void InstallStep();

public:
	FOnStepInstalledDelegate OnStepInstalled;


//Puzzle Section
public:
	void PuzzleDeactivate();
	void StartInstallPuzzle();


//Replicated Section
public:
	UPROPERTY(Replicated)
	uint8 bIsStepInstalled : 1;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSetVisibility();

	UFUNCTION(Server, Unreliable)
	void ServerRPCInstallStep();

	UFUNCTION(Server, Unreliable)
	void ServerRPCStartInstallPuzzle();
};
