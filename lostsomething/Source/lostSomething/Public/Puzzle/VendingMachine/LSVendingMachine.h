// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "Puzzle/VendingMachine/VendingMachineColor.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSVendingMachine.generated.h"

DECLARE_DELEGATE_OneParam(FOnVMPuzzleCheckDelegate, bool /*PuzzleCorrect*/);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSVendingMachine : public ALSInteractionActorBase
{
	GENERATED_BODY()
	
public:
	ALSVendingMachine();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere)
	TMap<EVendingMachineColor, class UMaterialInterface*> MeshMaterials;


//Interaction Section
public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;
	virtual void InteractionProcessSiJae(APlayerController* InPlayerController) override;
	virtual void InteractionProcessIJae(APlayerController* InPlayerController) override;

//Visible Section
protected:
	virtual void SetVisibleSiJae() override;
	virtual void SetVisibleIJae() override;

//Puzzle Section
public:
	FORCEINLINE void SetMachineNumber(int32 InMachineNumber) { MachineNumber = InMachineNumber; }
	FORCEINLINE void SetCurrentInteractController(APlayerController* InPlayerController) { CurrentInteractController = InPlayerController; }
	void BindVendingMachine(class ALSVendingMachineManager* InVendingMachineManager);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	EVendingMachineColor CurrentVendingMachineColor;

	FOnVMPuzzleCheckDelegate OnVMPuzzleCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	uint8 bisCorrectMachine : 1;

	UPROPERTY(Replicated)
	TObjectPtr<class APlayerController> CurrentInteractController;

	UPROPERTY(Replicated)
	uint8 bisPhaseStart:1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	int32 MachineNumber;

	TArray<TArray<EVendingMachineColor>> VendingMachineColorSets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	float DamageAmount;

	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();
	void SetMachineColor(EVendingMachineColor InAnswerColor, int32 InCurrentColorSet);
	void PuzzleCheck();
	void ApplyDamage();


//RPC Section
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCPuzzleCheck();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();

	UFUNCTION(Server, Unreliable)
	void ServerRPCSetCurrentInteractController(APlayerController* InPlayerController);

};
