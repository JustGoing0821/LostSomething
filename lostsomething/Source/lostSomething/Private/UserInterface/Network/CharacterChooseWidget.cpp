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

	TxtChoiceIJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceIJae")));
	ensure(TxtChoiceIJae);

	TxtChoiceSiJae = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_ChoiceSiJae")));
	ensure(TxtChoiceSiJae);

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
    auto PC = Cast<ALSCharacterChoiceController>(GetOwningPlayer());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
        return;
    }

    FString ServerNick = PC->ReceivedServerNickName.IsEmpty() ? TEXT("Host") : PC->ReceivedServerNickName;
    FString ClientNick = PC->ReceivedClientNickName.IsEmpty() ? TEXT("Guest") : PC->ReceivedClientNickName;

    UE_LOG(LogTemp, Warning, TEXT("UpdateWidget - Server: %s, Client: %s"), *ServerNick, *ClientNick);

    // 1. [시재(SiJae) 슬롯] 상태 결정
    if (ServerChoice == ELSCharacterChoice::SiJae)
    {
        TxtChoiceSiJae->SetText(FText::FromString(ServerNick));
    }
    else if (ClientChoice == ELSCharacterChoice::SiJae)
    {
        TxtChoiceSiJae->SetText(FText::FromString(ClientNick));
    }
    else
    {
        TxtChoiceSiJae->SetText(FText::GetEmpty());
    }

    // 2. [이재(IJae) 슬롯] 상태 결정
    if (ServerChoice == ELSCharacterChoice::IJae)
    {
        TxtChoiceIJae->SetText(FText::FromString(ServerNick));
    }
    else if (ClientChoice == ELSCharacterChoice::IJae)
    {
        TxtChoiceIJae->SetText(FText::FromString(ClientNick));
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

void UCharacterChooseWidget::OnClickedBtnGoLobby()
{
	UGameplayStatics::OpenLevel(this, FName("LobbyMap"));
}
