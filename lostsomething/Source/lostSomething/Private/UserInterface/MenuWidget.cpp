// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MenuWidget.h"
#include "Components/Button.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Game/LSGameMode.h"

void UMenuWidget::NativeConstruct()
{
	btn_Continue->OnClicked.AddDynamic(this, &UMenuWidget::ContinueGame);
	btn_Lobby->OnClicked.AddDynamic(this, &UMenuWidget::GoLobby);
	btn_ChooseMap->OnClicked.AddDynamic(this, &UMenuWidget::GoChooseMap);
	btn_Quit->OnClicked.AddDynamic(this, &UMenuWidget::QuitGame);
	
	GM = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode());
}

void UMenuWidget::ContinueGame()
{
	// 위젯 안보이게
	//이건 컨트롤러인가봄
}

void UMenuWidget::GoLobby()
{
	//둘다 로비 화면으로
	// LS 게임모드에서 로비로 가게 해야겠는데
	GM->MenuOption("Lobby");
}

void UMenuWidget::GoChooseMap()
{
	//맵 선택 화면?
	// LS 게임모드에서 로비로 가게 해야겠는데
	GM->MenuOption("ChooseMap");
}

void UMenuWidget::QuitGame()
{
	//종료
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
