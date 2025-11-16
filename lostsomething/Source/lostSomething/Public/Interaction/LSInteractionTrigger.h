// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSInteractionTrigger.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSInteractionTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSInteractionTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


//Trigger Section
protected:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> TriggerComponent;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


//Script Section
protected:
	UPROPERTY(EditAnywhere)
	FName ScriptAssetNameSiJae;

	UPROPERTY(EditAnywhere)
	FName ScriptAssetNameIJae;

	uint8 bSiJaeCanInteraction : 1;
	uint8 bIJaeCanInteraction : 1;

	UPROPERTY(EditAnywhere, Category = "LSCustom")
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptDataSiJae;

	UPROPERTY(EditAnywhere, Category = "LSCustom")
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptDataIJae;


//Quset Section
protected:
	UPROPERTY(Replicated)
	ELSInteractionEnum CurrentQuest;

	void SetCurrentQuest(ELSInteractionEnum InCurrentQuest);

	void BindQuestChange();

	UFUNCTION()
	virtual void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LSCustom, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum BlockDeactivateEnum;


//Block Section
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BlockBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> ViewMesh;


//RPC Section
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();

};
