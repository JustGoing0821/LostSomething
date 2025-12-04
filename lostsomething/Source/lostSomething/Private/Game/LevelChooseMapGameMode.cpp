// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LevelChooseMapGameMode.h"
#include "UserInterface/Network/LevelChooseWidget.h"

ALevelChooseMapGameMode::ALevelChooseMapGameMode()
{

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
	UE_LOG(LogTemp, Error, TEXT("ALevelChooseMapGameMode::BeginPlay()"));
}
