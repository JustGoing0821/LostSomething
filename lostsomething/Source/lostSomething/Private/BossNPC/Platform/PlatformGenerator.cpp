// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/PlatformGenerator.h"
#include <BossNPC/Platform/SpecialTile.h>
#include <Kismet/GameplayStatics.h>
#include <BossNPC/BossNPC.h>

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
	
    if (!BossNPC)
    {
        BossNPC = Cast<ABossNPC>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ABossNPC::StaticClass())
        );
    }
}

// Called every frame
void APlatformGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformGenerator::GenerateMaze()
{
	SpecialMap = GenerateConnectedSpecialPath();
    SpawnedColumns.Init(false, NumCols);
    SpawnTilesColumn(NumCols - 1);
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
    int32 NextCol = CurrentCol - 1;

    if (NextCol < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No more columns to spawn, CurrentCol=%d"), CurrentCol);
        return;
    }

    if (SpawnedColumns.IsValidIndex(CurrentCol) && SpawnedColumns[CurrentCol])
    {
        UE_LOG(LogTemp, Warning, TEXT("Column %d already stepped, skipping"), CurrentCol);
        return;
    }

    SpawnTilesColumn(NextCol);

    if (SpawnedColumns.IsValidIndex(CurrentCol))
    {
        SpawnedColumns[CurrentCol] = true;
    }
}

void APlatformGenerator::SpawnTilesColumn(int32 ColIndex)
{
    if (SpecialMap.Num() == 0 || ColIndex < 0 || ColIndex >= SpecialMap[0].Num()) return;
    if (!BossNPC) return;

    BossNPC->BMSoundPlay("Phase3");
    int32 Rows = SpecialMap.Num();

    FVector Forward = BossNPC->GetActorForwardVector();
    FVector Right = BossNPC->GetActorRightVector();
    FVector Origin = BossNPC->GetActorLocation();

    // 행 중앙 기준 좌우 오프셋 (왼쪽으로 땡기려면 음수)
    float RowOffset = (Rows - 1) * 0.5f * TileSpacing;

    for (int32 Row = Rows - 1; Row >= 0; --Row)
    {
        FVector SpawnLocation =
            Origin +
            Forward * (ColIndex * TileSpacing + 250.f) +
            Right * (Row * TileSpacing - RowOffset) +  // 여기서 좌측으로 땡김
            FVector(0, 0, -140.f);

        bool bIsSpecial = SpecialMap[Row][ColIndex];
        FRotator TileRotation = Forward.Rotation();

        AActor* TileActor = GetWorld()->SpawnActor<AActor>(
            bIsSpecial ? SpecialTileClass : NormalTileClass,
            SpawnLocation,
            TileRotation
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
