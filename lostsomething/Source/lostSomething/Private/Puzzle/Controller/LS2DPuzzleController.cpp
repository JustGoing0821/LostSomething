// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Controller/LS2DPuzzleController.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "Interface/LSSijaeCursorPosInterface.h"


ALS2DPuzzleController::ALS2DPuzzleController()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 60.0f;

	static ConstructorHelpers::FClassFinder<ULS2DPuzzleHUD> LS2DPuzzleHUDRef(TEXT("/Game/Level/Puzzle/UI/Blueprints/WBP_2DPuzzleHUD.WBP_2DPuzzleHUD_C"));
	if (LS2DPuzzleHUDRef.Class)
	{
		LS2DPuzzleHUDClass = LS2DPuzzleHUDRef.Class;
	}

	SetShowMouseCursor(true);
}

void ALS2DPuzzleController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALS2DPuzzleController, SiJaeCursorX);
	DOREPLIFETIME(ALS2DPuzzleController, SiJaeCursorY);
	DOREPLIFETIME(ALS2DPuzzleController, CharacterChoice);
}

void ALS2DPuzzleController::BeginPlay()
{
	if (CharacterChoice == ELSCharacterChoice::SiJae)
	{
		//SetInputMode(FInputModeGameAndUI());
	}
	else if (CharacterChoice == ELSCharacterChoice::IJae)
	{
		//SetInputMode(FInputModeGameOnly());
	}

	if (IsLocalController() && LS2DPuzzleHUDClass)
	{
		LS2DPuzzleHUDWidget = CreateWidget<ULS2DPuzzleHUD>(this, LS2DPuzzleHUDClass);
		if (LS2DPuzzleHUDWidget)
		{
			LS2DPuzzleHUDWidget->AddToViewport(0);
		}
	}

	//FTimerHandle Handle;
	//GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	//	{
	//		GetCurrentMousePos();
	//	}
	//), 1.f, true, 1.f);
}

void ALS2DPuzzleController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocalPlayerController() && CharacterChoice == ELSCharacterChoice::SiJae)
	{
		GetSiJaeLocalCursor();
	}

	if (HasAuthority())
	{
		FSijaeCursorPosData SijaeCursorPosData;

		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
		ILSSijaeCursorPosInterface* GameModeCursor = Cast<ILSSijaeCursorPosInterface>(GameMode);
		if (GameModeCursor)
		{
			SijaeCursorPosData = GameModeCursor->GetSijaeCursorPos();
		}

		SiJaeCursorX = SijaeCursorPosData.SiJaeCursorX;
		SiJaeCursorY = SijaeCursorPosData.SiJaeCursorY;
	}

	if (IsLocalPlayerController())
	{


		if (LS2DPuzzleHUDWidget)
		{
			LS2DPuzzleHUDWidget->SetCursorPosition(SiJaeCursorX, SiJaeCursorY);
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("No LS2DPuzzleHUDWidget"));
		}
	}
}

void ALS2DPuzzleController::SetGameModeSiJaeCursor(const FSijaeCursorPosData& InSijaeCursorPos)
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	ILSSijaeCursorPosInterface* GameModeCursor = Cast<ILSSijaeCursorPosInterface>(GameMode);
	if (GameModeCursor)
	{
		GameModeCursor->SetSijaeCursorPos(InSijaeCursorPos);
	}
}

void ALS2DPuzzleController::GetSiJaeLocalCursor()
{
	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);

	FSijaeCursorPosData SijaeCursorPosData = { MouseX , MouseY };

	if (HasAuthority())
	{
		SetGameModeSiJaeCursor(SijaeCursorPosData);
	}
	else
	{
		ServerRPCSetGameModeSiJaeCursor(SijaeCursorPosData);
	}
}

void ALS2DPuzzleController::ServerRPCSetGameModeSiJaeCursor_Implementation(const FSijaeCursorPosData& InSijaeCursorPos)
{
	SetGameModeSiJaeCursor(InSijaeCursorPos);
}

