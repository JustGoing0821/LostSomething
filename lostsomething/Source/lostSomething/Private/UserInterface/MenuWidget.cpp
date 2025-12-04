// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MenuWidget.h"
#include "Components/Button.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Game/LSGameMode.h"
#include "InputCoreTypes.h"

void UMenuWidget::NativeConstruct()
{
	btn_Continue->OnClicked.AddDynamic(this, &UMenuWidget::ContinueGame);
	btn_Lobby->OnClicked.AddDynamic(this, &UMenuWidget::GoLobby);
	btn_ChooseMap->OnClicked.AddDynamic(this, &UMenuWidget::GoChooseMap);
	btn_Quit->OnClicked.AddDynamic(this, &UMenuWidget::QuitGame);
	
	GM = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode());
}

bool UMenuWidget::Initialize()
{
	this->SetVisibility(ESlateVisibility::Hidden);
	return false;
}

void UMenuWidget::ContinueGame()
{
	// 위젯 안보이게
	//이건 컨트롤러인가봄
}

void UMenuWidget::GoLobby()
{
	GM->MenuOption("Lobby");
}

void UMenuWidget::GoChooseMap()
{
	GM->MenuOption("ChooseMap");
}

void UMenuWidget::QuitGame()
{
	//종료
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UMenuWidget::MenuVisibility()
{
	if (this->GetVisibility() == ESlateVisibility::Visible || this->GetVisibility() == ESlateVisibility::Hidden)
	{
		this->SetVisibility(ESlateVisibility::Hidden); // 또는 Hidden
	}
	else
	{
		this->SetVisibility(ESlateVisibility::Visible);
	}
}
