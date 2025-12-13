// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/CharacterChooseWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include <Game/LSGameInstance.h>
#include <Kismet/GameplayStatics.h>
#include <Game/LSCharacterChoiceController.h>

UCharacterChooseWidget::UCharacterChooseWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCharacterChooseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TxtChoiceIJaeServer = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceIJaeServer")));
	ensure(TxtChoiceIJaeServer);

	TxtChoiceSiJaeServer = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceSiJaeServer")));
	ensure(TxtChoiceSiJaeServer);

	TxtChoiceIJaeClient = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceIJaeClient")));
	ensure(TxtChoiceIJaeClient);

	TxtChoiceSiJaeClient = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceSiJaeClient")));
	ensure(TxtChoiceSiJaeClient);

	BtnSiJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_SiJae")));
	ensure(BtnSiJae);

	BtnIJae = Cast<UButton>(GetWidgetFromName(TEXT("btn_IJae")));
	ensure(BtnIJae);


	BtnGoLobby= Cast<UButton>(GetWidgetFromName(TEXT("btn_GoLobby")));
	ensure(BtnGoLobby);

	BtnSiJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthSiJae);
	BtnIJae->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBthIJae);
	BtnGoLobby->OnClicked.AddDynamic(this, &UCharacterChooseWidget::OnClickedBtnGoLobby);

}

void UCharacterChooseWidget::UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{
    const FSlateColor SelectedColor = FSlateColor(FLinearColor::White);

    FLinearColor DarkLinearColor = FLinearColor::FromSRGBColor(FColor::FromHex("2A2A3A"));
    DarkLinearColor.A = 0.8f;
    const FSlateColor UnselectedColor = FSlateColor(DarkLinearColor);

    if (ServerChoice == ELSCharacterChoice::SiJae)
    {
        TxtChoiceSiJaeServer->SetText(FText::FromString(TEXT("Host")));
        TxtChoiceSiJaeServer->SetColorAndOpacity(SelectedColor); // 흰색
    }
    else
    {
        TxtChoiceSiJaeServer->SetColorAndOpacity(UnselectedColor); // 어두운 색
    }

    if (ClientChoice == ELSCharacterChoice::SiJae)
    {
        TxtChoiceSiJaeClient->SetText(FText::FromString(TEXT("Guest")));
        TxtChoiceSiJaeClient->SetColorAndOpacity(SelectedColor); // 흰색
    }
    else
    {
        TxtChoiceSiJaeClient->SetColorAndOpacity(UnselectedColor); // 어두운 색
    }

    if (ServerChoice == ELSCharacterChoice::IJae)
    {
        TxtChoiceIJaeServer->SetText(FText::FromString(TEXT("Host")));
        TxtChoiceIJaeServer->SetColorAndOpacity(SelectedColor);
    }
    else
    {
        TxtChoiceIJaeServer->SetColorAndOpacity(UnselectedColor);
    }

    // (2) 클라이언트 텍스트
    if (ClientChoice == ELSCharacterChoice::IJae)
    {
        TxtChoiceIJaeClient->SetText(FText::FromString(TEXT("Guest")));
        TxtChoiceIJaeClient->SetColorAndOpacity(SelectedColor);
    }
    else
    {
        TxtChoiceIJaeClient->SetColorAndOpacity(UnselectedColor);
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

void UCharacterChooseWidget::OnClickedBtnGoLobby()
{
	UGameplayStatics::OpenLevel(this, FName("LobbyMap"));
}
