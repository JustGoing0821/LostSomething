// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "LSTrainStep.generated.h"

DECLARE_DELEGATE(FOnStepInstalledDelegate);
DECLARE_DELEGATE(FOnStepInstallFailedDelegate);

UCLASS()
class LOSTSOMETHING_API ALSTrainStep : public ALSInteractionActorBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSTrainStep();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

//Mesh Section
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

//Interaction Section
public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;
	virtual void InteractionProcessSiJae(APlayerController* InPlayerController) override;
	virtual void InteractionProcessIJae(APlayerController* InPlayerController) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> InteractionTrigger;


//StepInstall
protected:
	void InstallStep();

public:
	//FOnStepInstalledDelegate OnStepInstalled;
	//FOnStepInstallFailedDelegate OnStepInstallFailed;


//Puzzle Section
public:
	void PuzzleDeactivate();
	void StartInstallPuzzle();
	//void ClearInstallPuzzle();
	//void FailedInstallPuzzle();

protected:
	float PuzzleTimer;


//Replicated Section
public:
	UPROPERTY(Replicated)
	uint8 bIsStepInstalled : 1;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCInstallStep();

	UFUNCTION(Server, Unreliable)
	void ServerRPCInstallStep();

	UFUNCTION(Server, Unreliable)
	void ServerRPCStartInstallPuzzle();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCFailedInstallPuzzle();
};
