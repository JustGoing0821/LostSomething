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

	TxtServerChoiceIJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ServerChoiceIJae")));
	ensure(TxtServerChoiceIJae);

	TxtClientChoiceIJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ClientChoiceIJae")));
	ensure(TxtClientChoiceIJae);

	TxtServerChoiceSiJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ServerChoiceSiJae")));
	ensure(TxtServerChoiceSiJae);

	TxtClientChoiceSiJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ClientChoiceSiJae")));
	ensure(TxtClientChoiceSiJae);

	BtnSiJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_SiJae")));
	ensure(BtnSiJae);

	BtnIJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_IJae")));
	ensure(BtnIJae);

	BtnSiJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthSiJae);
	BtnIJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthIJae);
}

void UCharacterChooseWidget::UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{
	FColor sRGBColor = FColor::FromHex("2A2A3A");
	FLinearColor TargetColor = FLinearColor::FromSRGBColor(sRGBColor);

	if (ServerChoice == ELSCharacterChoice::SiJae)
	{
		TxtServerChoiceSiJae->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TxtServerChoiceIJae->SetColorAndOpacity(FSlateColor(TargetColor));
	}
	else if (ServerChoice == ELSCharacterChoice::IJae)
	{
		TxtServerChoiceIJae->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TxtServerChoiceSiJae->SetColorAndOpacity(FSlateColor(TargetColor));
	}

	if (ClientChoice == ELSCharacterChoice::SiJae)
	{
		TxtClientChoiceSiJae->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TxtClientChoiceIJae->SetColorAndOpacity(FSlateColor(TargetColor));
	}
	else if (ClientChoice == ELSCharacterChoice::IJae)
	{
		TxtClientChoiceIJae->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TxtClientChoiceSiJae->SetColorAndOpacity(FSlateColor(TargetColor));
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
