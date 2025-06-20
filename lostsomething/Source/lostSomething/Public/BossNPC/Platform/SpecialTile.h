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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* MeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ChangeVisible();

	void SetVisibleSiJae();
	void SetVisibleIJae();

	UPROPERTY()
	UMaterialInterface* Material_SiJae;

	UPROPERTY()
	UMaterialInterface* Material_IJae;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
