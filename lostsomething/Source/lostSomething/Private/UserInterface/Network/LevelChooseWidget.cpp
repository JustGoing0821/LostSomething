// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/LevelChooseWidget.h"
#include "Game/LSGameInstance.h"
#include "Components/Button.h"
#include "Game/LevelType.h"
#include <Game/LevelChooseMapGameMode.h>

void ULevelChooseWidget::NativeConstruct()
{
	GI = GetWorld()->GetGameInstance<ULSGameInstance>();

	btn_ChooseNewStart->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseNewStart);
	btn_ChooseStage1->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage1);
	btn_ChooseStage2->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage2);
	btn_ChooseStage3->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage3);
}

void ULevelChooseWidget::OnMyClicked_ChooseNewStart()
{
	MoveToCharacterChooseMap(ELevelType::NewStart);
}

void ULevelChooseWidget::OnMyClicked_ChooseStage1()
{
	MoveToCharacterChooseMap(ELevelType::Stage1);
}

void ULevelChooseWidget::OnMyClicked_ChooseStage2()
{
	MoveToCharacterChooseMap(ELevelType::Stage2);
}

void ULevelChooseWidget::OnMyClicked_ChooseStage3()
{
	MoveToCharacterChooseMap(ELevelType::Stage3);
}

void ULevelChooseWidget::MoveToCharacterChooseMap(ELevelType LevelType)
{
	if (!GI) return;

	GI->SetChooseLevel(LevelType);

	if (LevelType == ELevelType::Stage1)
	{
		GI->SetCurrentQuestIndex(4);
	}
	else if (LevelType == ELevelType::Stage2)
	{

		GI->SetCurrentQuestIndex(7);
	}
	else if (LevelType == ELevelType::Stage3)
	{
		GI->SetCurrentQuestIndex(9);
	}

	// GameMode 통해 이동
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->HasAuthority())
	{
		ALevelChooseMapGameMode* GM = Cast<ALevelChooseMapGameMode>(PC->GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->MoveToCharacterSelect();
		}
	}
}
