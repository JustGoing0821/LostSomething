// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionActor/LSInteractionActorBase.h"
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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UMaterialInterface> SiJaeMaterial;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UMaterialInterface> WrongMaterial;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UMaterialInterface> CorrectMaterial;

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
};
