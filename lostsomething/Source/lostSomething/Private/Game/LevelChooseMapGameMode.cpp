// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LevelChooseMapGameMode.h"
#include "UserInterface/Network/LevelChooseWidget.h"
#include "Game/LevelChoosePlayerController.h"

ALevelChooseMapGameMode::ALevelChooseMapGameMode()
{

}

void ALevelChooseMapGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    auto MyPC = Cast<ALevelChoosePlayerController>(NewPlayer);
    if (MyPC)
    {
        // 0.5초~1초 뒤에 부르도록 타이머 설정 (안전빵)
        FTimerHandle WaitHandle;
        GetWorld()->GetTimerManager().SetTimer(WaitHandle, [MyPC]()
            {
                if (MyPC && IsValid(MyPC)) // 포인터 유효성 체크
                {
                    MyPC->CreateLevelChooseWidget();
                }
            }, 1.0f, false); // 1초 뒤 실행
    }
}

void ALevelChooseMapGameMode::MoveToCharacterSelect()
{
    UWorld* World = GetWorld();
    if (World)
    {
        bool bResult = World->ServerTravel(TEXT("/Game/Map/ChooseMap?listen"), true, false);

        if (!bResult)
        {
            UE_LOG(LogTemp, Error, TEXT("ServerTravel Failed!"));
        }
    }
}

void ALevelChooseMapGameMode::BeginPlay()
{
}

void ALevelChooseMapGameMode::CheckAllPlayersReady_Level()
{
    int32 CurrentPlayerCount = GetNumPlayers();;

    if (CurrentPlayerCount >= RequiredPlayerCount)
    {
        for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            // Iterator->Get()으로 컨트롤러를 가져와서 내 컨트롤러 타입으로 캐스팅
            ALevelChoosePlayerController* PC = Cast<ALevelChoosePlayerController>(Iterator->Get());

            if (PC)
            {
                PC->CreateLevelChooseWidget();
            }
        }
    }
}
