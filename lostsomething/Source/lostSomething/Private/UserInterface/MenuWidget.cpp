// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MenuWidget.h"
#include "Components/Button.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Game/LSGameMode.h"
#include "Character/Players/LSPlayerController.h"

void UMenuWidget::NativeConstruct()
{
	if (btn_Continue)
		btn_Continue->OnClicked.AddDynamic(this, &UMenuWidget::ContinueGame);
	if (btn_Lobby)
		btn_Lobby->OnClicked.AddDynamic(this, &UMenuWidget::GoLobby);
	if (btn_ChooseMap)
		btn_ChooseMap->OnClicked.AddDynamic(this, &UMenuWidget::GoChooseMap);
	if (btn_Quit)
		btn_Quit->OnClicked.AddDynamic(this, &UMenuWidget::QuitGame);

	PC = Cast<ALSPlayerController>(GetOwningPlayer());
}

bool UMenuWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false; // 초기화 실패
	}

	this->SetVisibility(ESlateVisibility::Hidden);
	return false;
}

void UMenuWidget::ContinueGame()
{
	MenuVisibility();
	
}

void UMenuWidget::GoLobby()
{
	if (PC && PC->IsLocalController())
	{
		PC->MenuToLevel("Lobby");
	}
}

void UMenuWidget::GoChooseMap()
{
	if (PC && PC->IsLocalController())
	{
		PC->MenuToLevel("ChooseMap");
	}
}

void UMenuWidget::QuitGame()
{
	//종료
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UMenuWidget::MenuVisibility()
{

	if (this->GetVisibility() == ESlateVisibility::Visible)
	{
		// 메뉴를 숨기고 GameOnly 모드로 전환
		this->SetVisibility(ESlateVisibility::Hidden);
	}
	else // Hidden -> Visible
	{
		// 메뉴를 표시하고 GameAndUI 모드로 전환
		this->SetVisibility(ESlateVisibility::Visible);
	}
}
