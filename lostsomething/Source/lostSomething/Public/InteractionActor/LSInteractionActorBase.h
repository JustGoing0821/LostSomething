// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/LSInteractionInterface.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSInteractionActorBase.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSInteractionActorBase : public AActor, public ILSInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSInteractionActorBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;


//Interaction Section
public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;
	virtual void InteractionProcessSiJae() {};
	virtual void InteractionProcessIJae() {};

protected:
	UPROPERTY(Replicated)
	ELSInteractionEnum CurrentQuest;
	uint8 bSiJaeCanInteraction : 1;
	uint8 bIJaeCanInteraction : 1;

	void SetCurrentQuest(ELSInteractionEnum InCurrentQuest);

//Script Section
protected:
	FName ScriptAssetNameSiJae;
	FName ScriptAssetNameIJae;

	UPROPERTY(EditAnywhere, Category = ScriptData)
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptDataSiJae;

	UPROPERTY(EditAnywhere, Category = ScriptData)
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptDataIJae;

//Visible Section
protected:
	void ChangeVisible();
	virtual void SetVisibleSiJae() {};
	virtual void SetVisibleIJae() {};

//RPC Section
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCChangeVisible();
};
