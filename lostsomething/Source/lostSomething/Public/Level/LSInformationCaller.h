// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSInformationCaller.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSInformationCaller : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSInformationCaller();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom", Meta = (AllowPrivateAccess = "true"))
	bool bIsWaitPlayer;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> TriggerBox;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

// Quset Section
protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(enum ELSInteractionEnum InQuestEnum);
	void InformationActivate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom", Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;


// Script Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom", Meta = (AllowPrivateAccess = "true"))
	FName DataAssetName;


//RPC Section
public:

};
