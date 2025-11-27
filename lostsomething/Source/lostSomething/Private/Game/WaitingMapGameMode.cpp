// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WaitingMapGameMode.h"

AWaitingMapGameMode::AWaitingMapGameMode()
{
    bUseSeamlessTravel = false;
}

void AWaitingMapGameMode::PostLogin(APlayerController* NewPlayer)
{

    Super::PostLogin(NewPlayer);
    UE_LOG(LogTemp, Warning, TEXT("Current AWaitingMapGameMode"));
    CheckAllPlayersReady();
}

void AWaitingMapGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
}

void AWaitingMapGameMode::CheckAllPlayersReady()
{
    int32 CurrentPlayerCount = GetNumPlayers();;
    // 로그로 현재 상황 확실히 파악
    UE_LOG(LogTemp, Warning, TEXT("Current Players: %d / %d"), CurrentPlayerCount, RequiredPlayerCount);

    if (CurrentPlayerCount >= RequiredPlayerCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("All players ready! Moving to level select..."));

        // 타이머 중복 실행 방지 (이미 타이머가 돌고 있다면 패스)
        static bool bIsTravelScheduled = false;
        if (bIsTravelScheduled) return;
        bIsTravelScheduled = true;

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
            {
                UWorld* World = GetWorld();
                if (World)
                {
                    // 절대 경로 사용 및 에러 로그 방지
                    // bAbsolute = false (상대 경로), bShouldSkipGameNotify = false
                    bool bResult = World->ServerTravel(TEXT("/Game/Map/LevelChooseMap?listen"), true, false);

                    if (!bResult)
                    {
                        UE_LOG(LogTemp, Error, TEXT("ServerTravel Failed!"));
                    }
                }
            }, 1.0f, false);
    }
}
