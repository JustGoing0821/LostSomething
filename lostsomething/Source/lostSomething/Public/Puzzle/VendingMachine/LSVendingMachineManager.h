// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "Puzzle/VendingMachine/VendingMachineColor.h"
#include "Interaction/LSInteractionEnum.h"
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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVMPhaseChangedDelegate, EVendingMachineColor /*AnswerColor*/, int32 /*CurrentColorSet*/);
DECLARE_MULTICAST_DELEGATE(FOnVMPuzzleEndDelegate);

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
	TObjectPtr<class USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere)
	TMap<EVendingMachineColor, class UMaterialInterface*> MeshMaterials;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> StartButton;

	virtual void SetVisibleIJae() override;


//Interaction
public:
	virtual void InteractionProcessSiJae(APlayerController* InPlayerController) override;
	virtual void InteractionProcessIJae(APlayerController* InPlayerController) override;


//Puzzle Section
public:
	FOnVMPhaseChangedDelegate OnVMPhaseChanged;
	FOnVMPuzzleEndDelegate OnVMPuzzleEnd;

	UPROPERTY(Replicated)
	ECurrentPhase CurrentPhase;

	UPROPERTY(Replicated)
	EVendingMachineColor CurrentAnswerColor;

protected:
	int32 CurrentColorSet;
	TMap<ECurrentPhase, EVendingMachineColor> AnswerColors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;

	void StartPhase();
	void ProceedPhase();
	void PuzzleCheck(bool bisPuzzleCorrect);
	void QuestClear();
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();
	

//RPC Section
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCStartPhase();

	UFUNCTION(Server, Unreliable)
	void ServerRPCProceedPhase();

	UFUNCTION(Server, Unreliable)
	void ServerRPCQuestClear();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCQuestClear();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();
};
