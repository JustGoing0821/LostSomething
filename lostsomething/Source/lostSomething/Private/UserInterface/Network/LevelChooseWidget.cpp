// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/LevelChooseWidget.h"
#include "Game/LSGameInstance.h"
#include "Components/Button.h"
#include "Game/LevelType.h"

void ULevelChooseWidget::NativeConstruct()
{
	GI = GetWorld()->GetGameInstance<ULSGameInstance>();

	btn_ChooseNewStart->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseNewStart);
	btn_ChooseTutorial->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseTutorial);
	btn_ChooseStage1->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage1);
	btn_ChooseStage2->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage2);
	btn_ChooseStage3->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage3);
}

void ULevelChooseWidget::OnMyClicked_ChooseNewStart()
{
	GI->SetChooseLevel(ELevelType::NewStart);
}

void ULevelChooseWidget::OnMyClicked_ChooseTutorial()
{
	GI->SetChooseLevel(ELevelType::Tutorial);
}

void ULevelChooseWidget::OnMyClicked_ChooseStage1()
{
	GI->SetChooseLevel(ELevelType::Stage1);
}

void ULevelChooseWidget::OnMyClicked_ChooseStage2()
{
	GI->SetChooseLevel(ELevelType::Stage2);
}

void ULevelChooseWidget::OnMyClicked_ChooseStage3()
{
	GI->SetChooseLevel(ELevelType::Stage3);
}
