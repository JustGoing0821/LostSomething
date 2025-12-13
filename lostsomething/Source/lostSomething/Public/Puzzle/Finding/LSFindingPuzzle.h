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
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

// Interaction Section
protected:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;
	virtual void InteractionProcessSiJae(APlayerController* InPlayerController) override;
	virtual void InteractionProcessIJae(APlayerController* InPlayerController) override;

	virtual void SetVisibleSiJae() override;
	virtual void SetVisibleIJae() override;

// Puzzle Section
public:
	void SetPuzzleAnswer(uint8 bInCorrectPuzzle);

	FOnPuzzleCheckDelegate OnPuzzleCheck;

protected:
	void PuzzleCheck();

	UPROPERTY(VisibleAnywhere, Replicated)
	uint8 bIsCorrectPuzzle:1;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UMaterialInstanceDynamic> Material;

	TArray<TObjectPtr<class UTexture2D>> AnswerTextures;

	TArray<TObjectPtr<class UTexture2D>> WrongTextures;

	TArray<TObjectPtr<class UTexture2D>> SijaeTextures;

	uint8 bISTexturesUpdated : 1;


// Quest Section
protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;


// Damage Section
public:
	FORCEINLINE void SetCurrentInteractController(APlayerController* InPlayerController) { CurrentInteractController = InPlayerController; }

protected:
	UPROPERTY(Replicated)
	TObjectPtr<class APlayerController> CurrentInteractController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom", Meta = (AllowPrivateAccess = "true"))
	float DamageAmount;
	
	void ApplyDamage();


//RPC Section
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();

	UFUNCTION(Server, Unreliable)
	void ServerRPCSetCurrentInteractController(APlayerController* InPlayerController);

	UFUNCTION(Server, Unreliable)
	void ServerRPCPuzzleCheck();
};
