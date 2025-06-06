// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTPlayerController.h"
#include "Net/UnrealNetwork.h"

ALTPlayerController::ALTPlayerController()
{
}

void ALTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALTPlayerController, CharacterChoice);
}
