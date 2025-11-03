// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WaitingRoomGameMode.h"

AWaitingRoomGameMode::AWaitingRoomGameMode()
{

}

void AWaitingRoomGameMode::BeginPlay()
{

}

void AWaitingRoomGameMode::GameStart()
{
	GetWorld()->ServerTravel(TEXT("/Game/Stage/LSStage1Map1"));
}