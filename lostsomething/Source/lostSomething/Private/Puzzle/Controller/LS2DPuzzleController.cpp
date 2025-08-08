// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Controller/LS2DPuzzleController.h"
#include "Net/UnrealNetwork.h"

ALS2DPuzzleController::ALS2DPuzzleController()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SetShowMouseCursor(true);
}

void ALS2DPuzzleController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALS2DPuzzleController, SijaeMousePosX);
	DOREPLIFETIME(ALS2DPuzzleController, SijaeMousePosY);
}

void ALS2DPuzzleController::BeginPlay()
{
	if (CharacterChoice == ELSCharacterChoice::SiJae)
	{
		SetInputMode(FInputModeGameAndUI());
	}
	else if (CharacterChoice == ELSCharacterChoice::IJae)
	{
		SetInputMode(FInputModeGameOnly());
	}
}

void ALS2DPuzzleController::Tick(float DeltaTime)
{
	if (CharacterChoice == ELSCharacterChoice::SiJae)
	{
		if (HasAuthority())
		{
			GetMousePosition(SijaeMousePosX, SijaeMousePosY);
		}
		else
		{
			float MouseX, MouseY;
			GetMousePosition(MouseX, MouseY);
			ServerRPCSetMousePosition(MouseX, MouseY);
		}
	}
	else if(CharacterChoice == ELSCharacterChoice::IJae)
	{
		SetMouseLocation(SijaeMousePosX, SijaeMousePosY);
	}
}

void ALS2DPuzzleController::ServerRPCSetMousePosition_Implementation(float InMouseX, float InMouseY)
{
	SijaeMousePosX = InMouseX;
	SijaeMousePosY = InMouseY;
}

