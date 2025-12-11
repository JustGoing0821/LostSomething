// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/CharacterChooseWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UCharacterChooseWidget::UCharacterChooseWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCharacterChooseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TxtChoiceIJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceIJae")));
	ensure(TxtChoiceIJae);

	TxtChoiceSiJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceSiJae")));
	ensure(TxtChoiceSiJae);

	BtnSiJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_SiJae")));
	ensure(BtnSiJae);

	BtnIJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_IJae")));
	ensure(BtnIJae);

	BtnSiJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthSiJae);
	BtnIJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthIJae);
}

void UCharacterChooseWidget::UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{

	if (ServerChoice == ELSCharacterChoice::SiJae)
	{
		TxtChoiceSiJae->SetText(FText::FromString(TEXT("Host")));
	}
	else if (ServerChoice == ELSCharacterChoice::IJae)
	{
		TxtChoiceIJae->SetText(FText::FromString(TEXT("Host")));
	}

	if (ClientChoice == ELSCharacterChoice::SiJae)
	{
		TxtChoiceSiJae->SetText(FText::FromString(TEXT("Guest")));
	}
	else if (ClientChoice == ELSCharacterChoice::IJae)
	{
		TxtChoiceIJae->SetText(FText::FromString(TEXT("Guest")));
	}
	
}
 void UCharacterChooseWidget::OnClickedBthSiJae()
{
	OnCharacterChoose.ExecuteIfBound(ELSCharacterChoice::SiJae);
}

void UCharacterChooseWidget::OnClickedBthIJae()
{
	OnCharacterChoose.ExecuteIfBound(ELSCharacterChoice::IJae);
}
