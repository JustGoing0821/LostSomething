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
	// 1. [시재(SiJae) 슬롯] 상태 결정
	if (ServerChoice == ELSCharacterChoice::SiJae)
	{
		// 서버가 시재를 골랐으면 무조건 Host
		TxtChoiceSiJae->SetText(FText::FromString(TEXT("Host")));
	}
	else if (ClientChoice == ELSCharacterChoice::SiJae)
	{
		// 서버가 시재가 아닌데, 클라가 시재를 골랐으면 Guest
		TxtChoiceSiJae->SetText(FText::FromString(TEXT("Guest")));
	}
	else
	{
		// 아무도 시재를 안 골랐으면 빈카드로 (혹은 선택 가능 표시)
		TxtChoiceSiJae->SetText(FText::GetEmpty());
	}

	// 2. [이재(IJae) 슬롯] 상태 결정
	if (ServerChoice == ELSCharacterChoice::IJae)
	{
		TxtChoiceIJae->SetText(FText::FromString(TEXT("Host")));
	}
	else if (ClientChoice == ELSCharacterChoice::IJae)
	{
		TxtChoiceIJae->SetText(FText::FromString(TEXT("Guest")));
	}
	else
	{
		TxtChoiceIJae->SetText(FText::GetEmpty());
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
