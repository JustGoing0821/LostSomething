// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSItemSpawner.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:	
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> TriggerComponent;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AMasterItem>> WeaponItemArray;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AMasterItem>> ThrowItemArray;

	UPROPERTY(EditAnywhere, Category = "LSCustom")
	uint8 bIsWeapon:1;

	UPROPERTY(EditAnywhere, Category = "LSCustom")
	float SpawnTimer;

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SpawnItem();
};
