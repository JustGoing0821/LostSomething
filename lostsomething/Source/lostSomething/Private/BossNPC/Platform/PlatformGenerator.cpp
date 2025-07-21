// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/PlatformGenerator.h"
#include <BossNPC/Platform/SpecialTile.h>

// Sets default values
APlatformGenerator::APlatformGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

}

// Called when the game starts or when spawned
void APlatformGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlatformGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformGenerator::GenerateMaze()
{
	SpecialMap = GenerateConnectedSpecialPath();
	//SpawnTiles(SpecialMap);
    SpawnTilesColumn(0);
}

TArray<TArray<bool>> APlatformGenerator::GenerateConnectedSpecialPath()
{
    TArray<TArray<bool>> specialMap;
    specialMap.SetNum(NumRows);
    for (int32 Row = 0; Row < NumRows; ++Row)
        specialMap[Row].Init(false, NumCols);

    int32 CurrentCol = 0;
    int32 CurrentRow = FMath::RandRange(0, NumRows - 1);
    specialMap[CurrentRow][CurrentCol] = true;

    while (CurrentCol < NumCols - 1)
    {
        if (FMath::FRand() < 0.8f)
        {
            TArray<int32> VOptions;
            if (CurrentRow > 0) VOptions.Add(CurrentRow - 1);
            if (CurrentRow < NumRows - 1) VOptions.Add(CurrentRow + 1);

            if (VOptions.Num() > 0)
            {
                int32 NewRow = VOptions[FMath::RandRange(0, VOptions.Num() - 1)];

                int32 MinRow = FMath::Min(CurrentRow, NewRow);
                int32 MaxRow = FMath::Max(CurrentRow, NewRow);

                for (int32 Row = MinRow; Row <= MaxRow; ++Row)
                {
                    specialMap[Row][CurrentCol] = true;
                }

                CurrentRow = NewRow;
            }
        }

        int32 NextCol = CurrentCol + 1;
        specialMap[CurrentRow][NextCol] = true;

        CurrentCol = NextCol;
    }

    return specialMap;
}

void APlatformGenerator::OnSpecialTileStepped(int32 CurrentCol)
{
    int32 NextCol = CurrentCol + 1;
    if (NextCol < NumCols)
    {
        SpawnTilesColumn(NextCol);
    }
}

void APlatformGenerator::SpawnTilesColumn(int32 ColIndex)
{
    if (SpecialMap.Num() == 0) return;
    if (ColIndex >= SpecialMap[0].Num()) return;

    int32 Rows = SpecialMap.Num();

    FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 300.f;

    for (int32 Row = 0; Row < Rows; ++Row)
    {
        FVector SpawnLocation = StartLocation + FVector(ColIndex * TileSpacing, Row * TileSpacing, -200);

        bool bIsSpecial = SpecialMap[Row][ColIndex];

        TSubclassOf<AActor> TileClass = bIsSpecial ? SpecialTileClass : NormalTileClass;

        AActor* TileActor = GetWorld()->SpawnActor<AActor>(
            TileClass,
            SpawnLocation,
            FRotator::ZeroRotator
        );

        if (bIsSpecial)
        {
            if (ASpecialTile* SpecialTile = Cast<ASpecialTile>(TileActor))
            {
                SpecialTile->Init(this, ColIndex);
            }
        }
    }
}

void APlatformGenerator::SpawnTiles(const TArray<TArray<bool>>& specialMap)
{
    if (specialMap.Num() == 0) return;

    int32 Rows = specialMap.Num();
    int32 Cols = specialMap[0].Num();

    FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 300.f;

    for (int32 Row = 0; Row < Rows; ++Row)
    {
        for (int32 Col = 0; Col < Cols; ++Col)
        {
            FVector SpawnLocation;
            if (false)
            {
                // 전치된 배열이라서 X,Y 위치 계산을 바꿔야 함
                SpawnLocation = StartLocation + FVector(Row * TileSpacing, Col * TileSpacing, 0);
            }
            else
            {
                SpawnLocation = StartLocation + FVector(Col * TileSpacing, Row * TileSpacing, 0);
            }

            TSubclassOf<AActor> TileClass = specialMap[Row][Col] ? SpecialTileClass : NormalTileClass;
            GetWorld()->SpawnActor<AActor>(TileClass, SpawnLocation, FRotator::ZeroRotator);
        }
    }
}
