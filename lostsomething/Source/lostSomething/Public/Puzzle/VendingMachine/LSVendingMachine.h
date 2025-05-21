// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionActor/LSInteractionActorBase.h"
#include "Puzzle/VendingMachine/VendingMachineColor.h"
#include "LSVendingMachine.generated.h"

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
	virtual void InteractionProcessSiJae() override;
	virtual void InteractionProcessIJae() override;

//Visible Section
protected:
	virtual void SetVisibleSiJae() override;
	virtual void SetVisibleIJae() override;

//Quest Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	uint8 bisCorrectMachine : 1;

	void QuestClear();

//RPC Section
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCQuestClear();


//Puzzle Section
public:
	FORCEINLINE void SetMachineNumber(int32 InMachineNumber) { MachineNumber = InMachineNumber; }
	void BindOnPhaseChanged(class ALSVendingMachineManager* InVendingMAchineManager);

	UPROPERTY(Replicated)
	EVendingMachineColor CurrentVendingMachineColor;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	int32 MachineNumber;

	TArray<TArray<EVendingMachineColor>> VendingMachineColorSets;

	EVendingMachineColor AnswerColor;

	void SetMachineColor(EVendingMachineColor InAnswerColor, int32 InCurrentColorSet);
};
