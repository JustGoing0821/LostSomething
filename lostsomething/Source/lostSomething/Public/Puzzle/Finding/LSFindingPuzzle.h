// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSFindingPuzzle.generated.h"

DECLARE_DELEGATE_OneParam(FOnPuzzleCheckDelegate, uint8 /*bIsPuzzleCorrect*/);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSFindingPuzzle : public ALSInteractionActorBase
{
	GENERATED_BODY()
	
public:
	ALSFindingPuzzle();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

// Interaction Section
protected:
	virtual void InteractionProcessSiJae(APlayerController* InPlayerController) override;
	virtual void InteractionProcessIJae(APlayerController* InPlayerController) override;

	virtual void SetVisibleSiJae() override;
	virtual void SetVisibleIJae() override;

// Puzzle Section
public:
	void SetPuzzleAnswer(uint8 bInCorrectPuzzle);

	FOnPuzzleCheckDelegate OnPuzzleCheck;

protected:
	void PuzzleCheck(APlayerController* InPlayerController);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_bIsCorrectPuzzle)
	uint8 bIsCorrectPuzzle:1;

	float DamageAmount;

	UPROPERTY()
	class UMaterialInstance* CorrectMaterial;

	UPROPERTY()
	class UMaterialInstance* IncorrectMaterial;


// Quest Section
protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;


//RPC Section
public:
	UFUNCTION()
	void OnRep_bIsCorrectPuzzle();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();
};
