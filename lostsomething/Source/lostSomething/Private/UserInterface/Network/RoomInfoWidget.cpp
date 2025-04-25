// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/RoomInfoWidget.h"
#include "Game/LSGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void URoomInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_Join->OnClicked.AddDynamic(this, &URoomInfoWidget::OnMyJoinRoom);
}

void URoomInfoWidget::SetInfo(const FRoomInfo& info)
{
	RoomIndex = info.Index;

	txt_RoomName->SetText(FText::FromString(*info.RoomName));
	txt_HostName->SetText(FText::FromString(*info.HostName));
	txt_Ping->SetText(FText::FromString(*info.PingMS));
}

void URoomInfoWidget::OnMyJoinRoom()
{
	auto gi = Cast<ULSGameInstance>(GetWorld()->GetGameInstance());
	gi->JoinRoom(RoomIndex);
}
