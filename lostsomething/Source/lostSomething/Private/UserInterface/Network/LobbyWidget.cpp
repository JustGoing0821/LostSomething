// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/LobbyWidget.h"
#include "Game/LSGameInstance.h"
#include "UserInterface/Network/RoomInfoWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

const int SWITCHER_INDEX_MENU = 0;
const int SWITCHER_INDEX_CREATEROOM = 1;
const int SWITCHER_INDEX_FINDROOM = 2;

void ULobbyWidget::NativeConstruct()
{
	GI = GetWorld()->GetGameInstance<ULSGameInstance>();
	if (GI)
	{
		GI->OnAddRoomInfoDelegate.AddDynamic(this, &ULobbyWidget::AddRoomInfoWidget);
		GI->OnFindingRoomsDelegate.AddDynamic(this, &ULobbyWidget::SetFindActive);
	}

	if(btn_DoCreateRoom)
		btn_DoCreateRoom->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyClicked_doCreateRoom);
	if(btn_DoFindRoomList)
		btn_DoFindRoomList->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyDoFindRoomList);

	if(btn_GoCreateRoom)
		btn_GoCreateRoom->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyGoCreateRoom);
	//btn_DoVRCreateRoom->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyClicked_doCreateVRRoom);
	if(btn_GoFindRoom)
		btn_GoFindRoom->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyGoFindRoom);
	if(btn_GoMenuFromCreateRoom)
		btn_GoMenuFromCreateRoom->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyGoMenu);
	if(btn_GoMenuFromFindRoom)
		btn_GoMenuFromFindRoom->OnClicked.AddDynamic(this, &ULobbyWidget::OnMyGoMenu);

}

void ULobbyWidget::SwitchPanel(int32 index)
{
	WidgetSwitcherLobby->SetActiveWidgetIndex(index);
}

void ULobbyWidget::OnMyClicked_doCreateRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("ULobbyWidget::OnMyClicked_doCreateRoom()"));

	// 만약 방이름이 비어있으면 함수를 종료하고싶다.

	FString roomName = edit_RoomName->GetText().ToString();

	/**/
	if (roomName.TrimStartAndEnd().IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("방 이름을 안 적었어요!!"));
		return;
	}

	TArray<FString> badWords;
	badWords.Add(TEXT("바보"));
	badWords.Add(TEXT("똥개"));
	for (int i = 0; i < badWords.Num(); i++)
	{
		if (roomName.Contains(badWords[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("방 이름이 적절하지 않습니다."));
			return;
		}
	}

	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is null! Cannot create room"));
		return;
	}
	// UNetGameInstance::CreateRoom 를 호출하고싶다.
	GI->MyRoomName = roomName;
	GI->CreateRoom(roomName);
	
}

/*void ULobbyWidget::OnMyClicked_doCreateVRRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("ULobbyWidget::OnMyClicked_doCreateVRRoom()"));

	FString roomName = (TEXT("VR Room"));

	if (roomName.TrimStartAndEnd().IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("방 이름을 안 적었어요!!"));
		return;
	}

	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is null! Cannot create room"));
		return;
	}
	// UNetGameInstance::CreateRoom 를 호출하고싶다.
	GI->isVR = true;
	GI->MyRoomName = roomName;
	GI->CreateRoom(roomName);
}
*/


void ULobbyWidget::OnMyGoMenu()
{
	SwitchPanel(SWITCHER_INDEX_MENU);
}

void ULobbyWidget::OnMyGoCreateRoom()
{
	// 게임인스턴스가 있다면
	if (GI)
	{
		// 이름을 적었다면?
		if (false == edit_NickName->GetText().IsEmpty())
		{
			// 적은 이름을 내 닉네임으로 하고싶다.
			GI->NickName = edit_NickName->GetText().ToString();
		}
		else
		{
			// 기본 닉네임을 edit_nickName에 적고싶다.
			edit_NickName->SetText(FText::FromString(GI->NickName));
		}
		SwitchPanel(SWITCHER_INDEX_CREATEROOM);
	
	}
}

void ULobbyWidget::OnMyGoFindRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("ULobbyWidget::OnMyGoFindRoom()"));

	// 게임인스턴스가 있다면
	if (GI)
	{
		// 이름을 적었다면?
		if (false == edit_NickName->GetText().IsEmpty())
		{
			// 적은 이름을 내 닉네임으로 하고싶다.
			GI->NickName = edit_NickName->GetText().ToString();
		}
		else
		{
			// 기본 닉네임을 edit_nickName에 적고싶다.
			edit_NickName->SetText(FText::FromString(GI->NickName));
		}
		SwitchPanel(SWITCHER_INDEX_FINDROOM);
		// 메뉴에서 방찾기로 진입시에 조회를 하고싶다.
		OnMyDoFindRoomList();
	}
}

void ULobbyWidget::OnMyDoFindRoomList()
{
	scroll_RoomList->ClearChildren();
	if (GI)
	{
		GI->FindOtherRooms();
	}
}

void ULobbyWidget::AddRoomInfoWidget(const FRoomInfo& roomInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("AddRoomInfoWidget : begin"));

	if (nullptr == scroll_RoomList || nullptr == RoomInfoFactory)
		return;

	// 위젯을 생성해서 roomInfoFactory
	auto ui = CreateWidget<URoomInfoWidget>(GetWorld(), RoomInfoFactory);
	ui->SetInfo(roomInfo);
	// scroll_roomList의 자식으로 붙이고싶다.
	scroll_RoomList->AddChild(ui);

	UE_LOG(LogTemp, Warning, TEXT("AddRoomInfoWidget : end"));
}

void ULobbyWidget::SetFindActive(bool bActive)
{
	// bActive가 true라면
	if (!bActive)
	{
		// btn_doFindRoomList 버튼을 비활성화 하고싶다.
		btn_DoFindRoomList->SetIsEnabled(false);
		// txt_findingRooms를 보이게 하고싶다.
		txt_FindingRooms->SetVisibility(ESlateVisibility::Visible);
	}
	// 그렇지 않다면
	else {
		// btn_doFindRoomList 버튼을 활성화 하고싶다.
		btn_DoFindRoomList->SetIsEnabled(true);
		// txt_findingRooms를 안 보이게 하고싶다.
		txt_FindingRooms->SetVisibility(ESlateVisibility::Hidden);
	}
}
