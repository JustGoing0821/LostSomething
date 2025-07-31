// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSAttackTutorial.generated.h"

DECLARE_DELEGATE(FOnAttackTutorialDelegate);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSAttackTutorial : public ALSInteractionActorBase, public ILSTakeDamageInterface
{
	GENERATED_BODY()
	
public:
	ALSAttackTutorial();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	ELSCharacterChoice CorrectCauserCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FOnAttackTutorialDelegate OnAttackTutorial;

protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();


//RPC
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();

	UFUNCTION(Server, Unreliable)
	void ServerRPCPuzzleDeactivate();
};
