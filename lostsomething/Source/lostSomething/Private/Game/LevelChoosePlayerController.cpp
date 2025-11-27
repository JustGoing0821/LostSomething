// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LevelChoosePlayerController.h"
#include "UserInterface/Network/LevelChooseWidget.h"

ALevelChoosePlayerController::ALevelChoosePlayerController()
{
	static ConstructorHelpers::FClassFinder<ULevelChooseWidget> LevelChooseWidgetRef(TEXT("/Game/UI/Network/WBP_LevelChooseWidget.WBP_LevelChooseWidget_C"));
	if (LevelChooseWidgetRef.Class)
	{
		LevelChooseWidgetClass = LevelChooseWidgetRef.Class;
	}
}

void ALevelChoosePlayerController::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Error, TEXT("ALevelChooseMapGameMode::BeginPlay()"));

    if (IsLocalPlayerController() && HasAuthority())
    {
        if (LevelChooseWidgetClass)
        {
            LevelChooseWidget = CreateWidget<ULevelChooseWidget>(this, LevelChooseWidgetClass);
            if (LevelChooseWidget)
            {
                LevelChooseWidget->AddToViewport();

                // 마우스 커서 보이게 설정 (UI 맵이니까 필수)
                bShowMouseCursor = true;
                SetInputMode(FInputModeUIOnly());
            }
        }
    }
}
