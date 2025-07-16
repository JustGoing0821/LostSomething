// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpecialTile.generated.h"

UCLASS()
class LOSTSOMETHING_API ASpecialTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpecialTile();

	void Init(class APlatformGenerator* Generator, int32 ColIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* TriggerBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ChangeVisible();

	void SetVisibleSiJae();
	void SetVisibleIJae();

	UPROPERTY()
	UMaterialInterface* Material_SiJae;

	UPROPERTY()
	UMaterialInterface* Material_IJae;

	APlatformGenerator* PlatformGenerator;
	int32 ThisColumn;

	bool bOverlap;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
