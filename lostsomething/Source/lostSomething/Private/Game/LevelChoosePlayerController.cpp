// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LevelChoosePlayerController.h"
#include "UserInterface/Network/LevelChooseWidget.h"
#include <Game/LevelChooseMapGameMode.h>
#include <Kismet/GameplayStatics.h>

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

void ALevelChoosePlayerController::Server_NotifyWidgetReady_Implementation()
{
    auto GM = Cast<ALevelChooseMapGameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        GM->AddReadyPlayerCount(); // 게임모드 함수 호출
    }
}

void ALevelChoosePlayerController::Client_UnlockButton_Implementation()
{
    LevelWidgetInstance->SetupInputPermission(HasAuthority());
}

void ALevelChoosePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelChoosePlayerController::Client_CreateLevelChooseWidget_Implementation()
{
    LevelWidgetInstance = CreateWidget<ULevelChooseWidget>(this, LevelChooseWidget);
    if (LevelWidgetInstance)
    {
        LevelWidgetInstance->AddToViewport();

        // 마우스 커서 보이게 설정 (UI 맵이니까 필수)
        bShowMouseCursor = true;
        SetInputMode(FInputModeUIOnly());
    }
}
