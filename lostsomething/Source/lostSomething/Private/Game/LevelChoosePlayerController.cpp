// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LevelChoosePlayerController.h"
#include "UserInterface/Network/LevelChooseWidget.h"

ALevelChoosePlayerController::ALevelChoosePlayerController()
{
    bReplicates = true;
}

void ALevelChoosePlayerController::CreateLevelChooseWidget()
{
    if (HasAuthority())
    {
        Client_CreateLevelChooseWidget();
    }
}

void ALevelChoosePlayerController::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Error, TEXT("ALevelChoosePlayerController::BeginPlay()"));

}

void ALevelChoosePlayerController::Client_CreateLevelChooseWidget_Implementation()
{
    auto ui = CreateWidget<ULevelChooseWidget>(this, LevelChooseWidget);
    if (ui)
    {
        ui->AddToViewport();

        // 마우스 커서 보이게 설정 (UI 맵이니까 필수)
        bShowMouseCursor = true;
        SetInputMode(FInputModeUIOnly());

        bool bIsServer = HasAuthority();
        ui->SetupInputPermission(bIsServer);
    }
}
