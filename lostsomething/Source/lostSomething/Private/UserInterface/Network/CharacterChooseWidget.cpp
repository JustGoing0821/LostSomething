// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/CharacterChooseWidget.h"
#include "lostSomething.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UCharacterChooseWidget::UCharacterChooseWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCharacterChooseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TxtServerChoice = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ServerChoice")));
	ensure(TxtServerChoice);

	TxtClientChoice = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ClientChoice")));
	ensure(TxtClientChoice);

	BtnSiJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_SiJae")));
	ensure(BtnSiJae);

	BtnIJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_IJae")));
	ensure(BtnIJae);

	BtnSiJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthSiJae);
	BtnIJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthIJae);
}

void UCharacterChooseWidget::UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (ServerChoice == ELSCharacterChoice::SiJae)
	{
		TxtServerChoice->SetText(FText::FromString(TEXT("SiJae")));
	}
	else if (ServerChoice == ELSCharacterChoice::IJae)
	{
		TxtServerChoice->SetText(FText::FromString(TEXT("IJae")));
	}

	if (ClientChoice == ELSCharacterChoice::SiJae)
	{
		TxtClientChoice->SetText(FText::FromString(TEXT("SiJae")));
	}
	else if (ClientChoice == ELSCharacterChoice::IJae)
	{
		TxtClientChoice->SetText(FText::FromString(TEXT("IJae")));
	}
	
}

void UCharacterChooseWidget::OnClickedBthSiJae()
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	OnCharacterChoose.ExecuteIfBound(ELSCharacterChoice::SiJae);
}

void UCharacterChooseWidget::OnClickedBthIJae()
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	OnCharacterChoose.ExecuteIfBound(ELSCharacterChoice::IJae);
}
