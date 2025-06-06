// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/LSInteractionInterface.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSItemTest.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSItemTest : public AActor, public ILSInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSItemTest();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

//Interaction Section
public:
	virtual void InteractionProcess(APlayerController* InPlayerController) override;

protected:
	ELSInteractionEnum CurrentQuest;
	uint8 bCanInteraction : 1;

//Script Section
protected:
	UPROPERTY(EditAnywhere, Category = ScriptData)
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptData;

	FName ScriptAssetName;
};
