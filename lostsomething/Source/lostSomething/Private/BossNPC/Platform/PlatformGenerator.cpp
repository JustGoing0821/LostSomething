// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/PlatformGenerator.h"

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
	TArray<TArray<bool>> specialMap = GenerateConnectedSpecialPath();
	SpawnTiles(specialMap);
}

TArray<TArray<bool>> APlatformGenerator::GenerateConnectedSpecialPath()
{
    TArray<TArray<bool>> specialMap;
    specialMap.SetNum(NumRows);
    for (int32 Row = 0; Row < NumRows; ++Row)
        specialMap[Row].Init(false, NumCols);

    struct Coord { int32 Row, Col; };
    TArray<Coord> Path;

    // 1. 시작점 선택
    int32 StartCol = FMath::RandRange(0, NumCols - 1);
    Path.Add({ 0, StartCol });
    specialMap[0][StartCol] = true;

    // 2. 경로 생성 (길이 >= NumRows)
    while (true)
    {
        Coord Current = Path.Last();

        // 상하좌우 후보
        TArray<Coord> Neighbors;
        if (Current.Row > 0 && !specialMap[Current.Row - 1][Current.Col])
            Neighbors.Add({ Current.Row - 1, Current.Col });
        if (Current.Row < NumRows - 1 && !specialMap[Current.Row + 1][Current.Col])
            Neighbors.Add({ Current.Row + 1, Current.Col });
        if (Current.Col > 0 && !specialMap[Current.Row][Current.Col - 1])
            Neighbors.Add({ Current.Row, Current.Col - 1 });
        if (Current.Col < NumCols - 1 && !specialMap[Current.Row][Current.Col + 1])
            Neighbors.Add({ Current.Row, Current.Col + 1 });

        if (Neighbors.Num() == 0)
            break; // 경로가 더 이상 확장 불가

        Coord Next = Neighbors[FMath::RandRange(0, Neighbors.Num() - 1)];
        specialMap[Next.Row][Next.Col] = true;
        Path.Add(Next);

        // 경로가 마지막 행에 도달하면 종료
        if (Next.Row == NumRows - 1)
            break;
    }

    // 3. 추가 특수 발판 랜덤으로 생성 (기존 경로 주변)
    for (int32 Row = 0; Row < NumRows; ++Row)
    {
        for (int32 Col = 0; Col < NumCols; ++Col)
        {
            if (specialMap[Row][Col])
                continue;

            // 주변에 특수 발판이 있는 경우만 고려
            bool bNeighborHasSpecial = false;
            if (Row > 0 && specialMap[Row - 1][Col]) bNeighborHasSpecial = true;
            if (Row < NumRows - 1 && specialMap[Row + 1][Col]) bNeighborHasSpecial = true;
            if (Col > 0 && specialMap[Row][Col - 1]) bNeighborHasSpecial = true;
            if (Col < NumCols - 1 && specialMap[Row][Col + 1]) bNeighborHasSpecial = true;

            if (bNeighborHasSpecial && FMath::FRand() < 0.5f) // 확률적으로 추가
                specialMap[Row][Col] = true;
        }
    }

    return specialMap;
}

void APlatformGenerator::SpawnTiles(const TArray<TArray<bool>>& specialMap)
{
    FVector StartLocation = GetActorLocation();

    for (int32 Row = 0; Row < NumRows; ++Row)
    {
        for (int32 Col = 0; Col < NumCols; ++Col)
        {
            FVector SpawnLocation = StartLocation + FVector(Col * TileSpacing, Row * TileSpacing, 0);
            TSubclassOf<AActor> TileClass = specialMap[Row][Col] ? SpecialTileClass : NormalTileClass;
            GetWorld()->SpawnActor<AActor>(TileClass, SpawnLocation, FRotator::ZeroRotator);
        }
    }
}
