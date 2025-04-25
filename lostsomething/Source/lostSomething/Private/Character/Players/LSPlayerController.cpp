// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerController.h"

void ALSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
