// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformGenerator.generated.h"

UCLASS()
class LOSTSOMETHING_API APlatformGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> NormalTileClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> SpecialTileClass;

    UPROPERTY(EditAnywhere)
    float TileSpacing = 200.f;

    UPROPERTY(EditAnywhere)
    int32 NumRows = 5;

    UPROPERTY(EditAnywhere)
    int32 NumCols = 9;

    void GenerateMaze();
    TArray<TArray<bool>> GenerateConnectedSpecialPath();
    void SpawnTiles(const TArray<TArray<bool>>& specialMap);

};
