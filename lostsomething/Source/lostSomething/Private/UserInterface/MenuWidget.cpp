// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MenuWidget.h"
#include "Components/Button.h"

void UMenuWidget::NativeConstruct()
{
	btn_Continue->OnClicked.AddDynamic(this, &UMenuWidget::ContinueGame);
	btn_Lobby->OnClicked.AddDynamic(this, &UMenuWidget::GoLobby);
	btn_ChooseMap->OnClicked.AddDynamic(this, &UMenuWidget::GoChooseMap);
	btn_Quit->OnClicked.AddDynamic(this, &UMenuWidget::QuitGame);
}

void UMenuWidget::ContinueGame()
{
	// 위젯 안보이게
}

void UMenuWidget::GoLobby()
{
	//둘다 로비 화면으로
}

void UMenuWidget::GoChooseMap()
{
	//맵 선택 화면?
}

void UMenuWidget::QuitGame()
{
	//종료
}
