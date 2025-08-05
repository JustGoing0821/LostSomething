// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/LSInteractionInterface.h"
#include "LSQuestClearHack.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSQuestClearHack : public AActor, public ILSInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSQuestClearHack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;

protected:
	void QuestClear();

	UPROPERTY(EditAnywhere)
	bool bIsStartClear;

//RPC Section
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCQuestClear();

};
