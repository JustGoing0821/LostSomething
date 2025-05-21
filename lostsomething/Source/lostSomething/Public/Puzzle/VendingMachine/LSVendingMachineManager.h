// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionActor/LSInteractionActorBase.h"
#include "Puzzle/VendingMachine/VendingMachineColor.h"
#include "LSVendingMachineManager.generated.h"

UENUM(BlueprintType)
enum class ECurrentPhase : uint8
{
	NotStarted =0,
	Phase1,
	Phase2,
	Phase3,
	Phase4
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPhaseChangedDelegate, EVendingMachineColor /*AnswerColor*/, int32 /*CurrentColorSet*/);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSVendingMachineManager : public ALSInteractionActorBase
{
	GENERATED_BODY()
	
public:
	ALSVendingMachineManager();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


//Pannel Section
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere)
	TMap<EVendingMachineColor, class UMaterialInterface*> MeshMaterials;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> StartButton;

	virtual void SetVisibleIJae() override;


//Interaction
public:
	virtual void InteractionProcessSiJae() override;
	virtual void InteractionProcessIJae() override;


//Vending Machine Section
protected:
	TArray<class ALSVendingMachine*> VendingMachines;


//Puzzle Section
public:
	FOnPhaseChangedDelegate OnPhaseChanged;

	UPROPERTY(Replicated)
	ECurrentPhase CurrentPhase;

	UPROPERTY(Replicated)
	EVendingMachineColor CurrentAnswerColor;

protected:
	int32 CurrentColorSet;
	TMap<ECurrentPhase, EVendingMachineColor> AnswerColors;

	void StartPhase();
	

//RPC Section
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCStartPhase();
};
