// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "LSStepInteraction.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSStepInteraction : public ALSInteractionActorBase
{
	GENERATED_BODY()
	
public:
	ALSStepInteraction();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;
	virtual void InteractionProcessSiJae(APlayerController* InPlayerController) override;
	virtual void InteractionProcessIJae(APlayerController* InPlayerController) override;

	FORCEINLINE uint8 GetbIsStepGetted() { return bIsStepGetted; }

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(Replicated)
	uint8 bIsStepGetted : 1;
};
