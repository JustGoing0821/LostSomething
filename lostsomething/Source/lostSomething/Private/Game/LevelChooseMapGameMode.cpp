// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LevelChooseMapGameMode.h"
#include "UserInterface/Network/LevelChooseWidget.h"

ALevelChooseMapGameMode::ALevelChooseMapGameMode()
{
	//Widget
	static ConstructorHelpers::FClassFinder<ULevelChooseWidget> LevelChooseWidgetRef(TEXT("/Game/UI/Network/WBP_LevelChooseWidget.WBP_LevelChooseWidget_C"));
	if (LevelChooseWidgetRef.Class)
	{
		LevelChooseWidgetClass = LevelChooseWidgetRef.Class;
	}
}

void ALevelChooseMapGameMode::MoveToCharacterSelect()
{
	GetWorld()->ServerTravel("/Game/Maps/CharacterChooseMap?listen");
}

void ALevelChooseMapGameMode::BeginPlay()
{
	if (LevelChooseWidgetClass)
	{
		LevelChooseWidget = CreateWidget<ULevelChooseWidget>(GetWorld(), LevelChooseWidgetClass);
		if (LevelChooseWidget)
		{
			LevelChooseWidget->AddToViewport(0);
		}
	}
}
