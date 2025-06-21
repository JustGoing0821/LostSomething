// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/ChooseCharacterEndWidget.h"
#include "lostSomething.h"
#include "Components/Button.h"

UChooseCharacterEndWidget::UChooseCharacterEndWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UChooseCharacterEndWidget::NativeConstruct()
{
	BtnGameStart = Cast<UButton>(GetWidgetFromName(TEXT("btn_GameStart")));
	ensure(BtnGameStart);

	BtnGameStart->OnClicked.AddDynamic(this, &UChooseCharacterEndWidget::OnClickedBtnGameStart);
}

void UChooseCharacterEndWidget::OnClickedBtnGameStart()
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	OnCharacterChooseEnd.ExecuteIfBound();
}
