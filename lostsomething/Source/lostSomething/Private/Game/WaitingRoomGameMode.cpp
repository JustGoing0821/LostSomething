// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WaitingRoomGameMode.h"
#include <lostSomething.h>
#include "UserInterface/Network/VRReadyWidget.h"
#include <Kismet/GameplayStatics.h>

AWaitingRoomGameMode::AWaitingRoomGameMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FInputModeUIOnly InputMode;
		//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

		LS_LOG(LogLS, Warning, TEXT("Input Mode changed to UI Only"));
	}
}

void AWaitingRoomGameMode::BeginPlay()
{

	Super::BeginPlay();

	if (VRReadyWidgetClass)
	{
		VRReadyWidget = CreateWidget<UVRReadyWidget>(GetWorld(), VRReadyWidgetClass);
		if (VRReadyWidget)
		{
			VRReadyWidget->AddToViewport(0);
			//GameStartWidget->OnCharacterChooseEnd.BindUObject(this, &ALSCharacterChooseGameMode::GameStart);
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("%s"), TEXT("GameStartWidget Null"));
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("GameStartWidgetClass Null"));
	}
}

void AWaitingRoomGameMode::GameStart()
{
	GetWorld()->ServerTravel(TEXT("/Game/Stage/LSStage1Map1"));
}