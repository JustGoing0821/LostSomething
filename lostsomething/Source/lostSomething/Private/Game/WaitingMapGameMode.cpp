// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WaitingMapGameMode.h"

void AWaitingMapGameMode::PostLogin(APlayerController* NewPlayer)
{

    Super::PostLogin(NewPlayer);

    CheckAllPlayersReady();
}

void AWaitingMapGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
}

void AWaitingMapGameMode::CheckAllPlayersReady()
{
    int32 CurrentPlayerCount = GetNumPlayers();;
    if (CurrentPlayerCount >= RequiredPlayerCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("All players ready! Moving to level select..."));

        // 약간의 딜레이 후 레벨 이동 (UI 표시 등을 위해)
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
            {
                // 서버만 레벨 선택 맵으로 이동
                GetWorld()->ServerTravel("/Game/Maps/LevelChooseMap?listen");
            }, 2.0f, false);
    }
}
