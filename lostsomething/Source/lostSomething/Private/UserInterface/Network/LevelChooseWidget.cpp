// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/LevelChooseWidget.h"
#include "Game/LSGameInstance.h"
#include "Components/Button.h"
#include "Game/LevelType.h"
#include <Game/LevelChooseMapGameMode.h>
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include <Game/LevelChoosePlayerController.h>

void ULevelChooseWidget::NativeConstruct()
{
	GI = GetWorld()->GetGameInstance<ULSGameInstance>();

	btn_ChooseNewStart->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseNewStart);
	btn_ChooseStage1->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage1);
	btn_ChooseStage2->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage2);
	btn_ChooseStage3->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_ChooseStage3);
	btn_GoLobby->OnClicked.AddDynamic(this, &ULevelChooseWidget::OnMyClicked_GoLobby);

	if (btn_ChooseNewStart) btn_ChooseNewStart->SetIsEnabled(false);
	if (btn_ChooseStage1) btn_ChooseStage1->SetIsEnabled(false);
	if (btn_ChooseStage2) btn_ChooseStage2->SetIsEnabled(false);
	if (btn_ChooseStage3) btn_ChooseStage3->SetIsEnabled(false);

	if (txt_StatusMessage)
	{
		txt_StatusMessage->SetText(FText::FromString(TEXT("Please Select a Map")));
	}

	auto PC = Cast<ALevelChoosePlayerController>(GetOwningPlayer());
	if (PC)
	{
		// 3. 컨트롤러에게 "나 준비됐으니 서버에 알려줘"라고 요청
		PC->Server_NotifyWidgetReady();
	}
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

void ULevelChooseWidget::OnMyClicked_GoLobby()
{
	GI->ExitRoom();
	UGameplayStatics::OpenLevel(this, FName("LobbyMap"));
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

void ULevelChooseWidget::SetupInputPermission(bool bCanSelect)
{
	if (bCanSelect)
	{
		if (btn_ChooseNewStart) btn_ChooseNewStart->SetIsEnabled(true);
		if (btn_ChooseStage1) btn_ChooseStage1->SetIsEnabled(true);
		if (btn_ChooseStage2) btn_ChooseStage2->SetIsEnabled(true);
		if (btn_ChooseStage3) btn_ChooseStage3->SetIsEnabled(true);

		if (txt_StatusMessage)
		{
			txt_StatusMessage->SetText(FText::FromString(TEXT("Please Select a Map")));
		}
	}
	else
	{
		if (btn_ChooseNewStart) btn_ChooseNewStart->SetIsEnabled(false);
		if (btn_ChooseStage1) btn_ChooseStage1->SetIsEnabled(false);
		if (btn_ChooseStage2) btn_ChooseStage2->SetIsEnabled(false);
		if (btn_ChooseStage3) btn_ChooseStage3->SetIsEnabled(false);

		if (txt_StatusMessage)
		{
			txt_StatusMessage->SetText(FText::FromString(TEXT("Host is selecting the map...")));
		}
	}

}