// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSMediaPlayerTrigger.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSMediaPlayerTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSMediaPlayerTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom")
	bool bIsMapStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom")
	bool bisNeedQuestComplete;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LSCustom")
	TObjectPtr<class UFileMediaSource> MediaSource;

protected:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
