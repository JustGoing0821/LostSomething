// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LobbyGameMode.h"
#include "lostSomething.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	LS_LOG(LogLS, Error, TEXT("Last Edit Serial Num : %d"), 909426);
}
