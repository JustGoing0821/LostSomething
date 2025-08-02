// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSMoveTutorial.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSMoveTutorial : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSMoveTutorial();

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> TutorialTrigger;

	int32 CurrentTriggerPlayers;

	UFUNCTION()
	void OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();
	void QuestClear();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;

//Script Section
protected:
	FName ScriptAssetNameSiJae;
	FName ScriptAssetNameIJae;

	UPROPERTY(EditAnywhere, Category = ScriptData)
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptDataSiJae;

	UPROPERTY(EditAnywhere, Category = ScriptData)
	TObjectPtr<class ULSInteractionScriptData> InteractionScriptDataIJae;

//RPC
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPuzzleDeactivate();

};
