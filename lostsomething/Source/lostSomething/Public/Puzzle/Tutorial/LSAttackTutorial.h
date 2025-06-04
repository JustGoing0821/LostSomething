// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionActor/LSInteractionActorBase.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Character/Players/LSCharacterChoice.h"
#include "LSAttackTutorial.generated.h"



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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	ELSCharacterChoice CorrectCauserCharacter;
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	void QuestClear();

//RPC
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCQuestClear();
};
