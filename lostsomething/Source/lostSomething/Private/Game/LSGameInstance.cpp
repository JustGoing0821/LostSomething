// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include <string>


void ULSGameInstance::Init()
{
	Super::Init();

	if (auto Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();

		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULSGameInstance::OnMyCreateRoomComplete);

		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ULSGameInstance::OnMyFindOtherRoomsComplete);

		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ULSGameInstance::OnMyJoinRoomComplete);

		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ULSGameInstance::OnMyExitRoomComplete);
	}
}

void ULSGameInstance::CreateRoom(FString RoomName)
{
	FOnlineSessionSettings Setting;

	// 1. 데디케이트 서버인가??
	Setting.bIsDedicated = false;
	// 2. 랜선인가?
	auto SubSys = IOnlineSubsystem::Get();
	Setting.bIsLANMatch = SubSys->GetSubsystemName().IsEqual("NULL");
	// 3. 공개로 입장할 수 있는가? 아니면 친구초대로만???
	Setting.bShouldAdvertise = true;
	// 4. 온라인 상태(presence)를 공개적으로 사용할것인가? -> ping정보
	Setting.bUsesPresence = true;
	// 5. 중간입장이 가능한가?
	Setting.bAllowJoinInProgress = true;
	Setting.bAllowJoinViaPresence = true;
	// 6. 최대 입장 가능한 수 설정
	Setting.NumPublicConnections = 2;
	// 7. 커스텀 정보 설정


	Setting.Set(TEXT("ROOM_NAME"), StringBase64Encode(RoomName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("HOST_NAME"), StringBase64Encode(NickName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	// 8. netID 찾기
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	UE_LOG(LogTemp, Warning, TEXT("CreateRoom Start!!! roomNamd : %s, netID : %s"), *RoomName, *netID->ToString());

	SessionInterface->CreateSession(*netID, FName(*RoomName), Setting);

}

void ULSGameInstance::OnMyCreateRoomComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMyCreateRoomComplete!!! sessionName : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);

	// 방을 생성했다면
	if (bWasSuccessful)
	{
		// 입장한 방의 이름을 기억하고싶다.
		MyRoomName = SessionName.ToString();
		// 서버는 세계 여행을 떠나고싶다. 어디로???
		FString url = TEXT("/Game/Map/ChooseMap?listen");
		GetWorld()->ServerTravel(url);
	}
}

void ULSGameInstance::FindOtherRooms()
{
	// 1. FOnlineSessionSearch객체를 생성
	RoomSearch = MakeShareable(new FOnlineSessionSearch());
	// 2. 세션 검색 조건 설정
	RoomSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	// 3. 최대 검색 갯수를 정하고싶다.
	RoomSearch->MaxSearchResults = 3;
	// 4. 랜선인지 아닌지를 정하고싶다.
	auto subSys = IOnlineSubsystem::Get();
	RoomSearch->bIsLanQuery = subSys->GetSubsystemName().IsEqual("NULL");

	// 5. 검색을 하고싶다.
	SessionInterface->FindSessions(0, RoomSearch.ToSharedRef());

	if (OnFindingRoomsDelegate.IsBound())
	{
		OnFindingRoomsDelegate.Broadcast(true);
	}
}

void ULSGameInstance::OnMyJoinRoomComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result)
{
	// 성공했다면?
	if (EOnJoinSessionCompleteResult::Success == result)
	{
		// 입장한 방의 이름을 기억하고싶다.
		MyRoomName = SessionName.ToString();

		// 서버의 주소를 받아와서
		FString url;
		SessionInterface->GetResolvedConnectString(SessionName, url);
		// 여행을 떠나고 싶다.
		auto pc = GetWorld()->GetFirstPlayerController();
		if(pc)
			pc->ClientTravel(url, TRAVEL_Absolute);
	}
	// 그렇지않다면
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Failed... : %d"), result);
	}
}

void ULSGameInstance::OnMyFindOtherRoomsComplete(bool bWasSuccesful)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), bWasSuccesful);
	for (auto r : RoomSearch->SearchResults)
	{
		if (false == r.IsValid())
			continue;

		FString roomName;
		r.Session.SessionSettings.Get(TEXT("ROOM_NAME"), roomName);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *roomName);
	}
}

void ULSGameInstance::JoinRoom(int32 index)
{
	auto r = RoomSearch->SearchResults[index];
	FString sessionName;
	r.Session.SessionSettings.Get(TEXT("ROOM_NAME"), sessionName);
	SessionInterface->JoinSession(0, FName(*sessionName), r);
}

bool ULSGameInstance::IsInRoom()
{
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	return SessionInterface->IsPlayerInSession(FName(*MyRoomName), *netID);

}

void ULSGameInstance::ExitRoom()
{
	ServerExitRoom();
}

void ULSGameInstance::ServerExitRoom_Implementation()
{
	MultiExitRoom();
}

void ULSGameInstance::MultiExitRoom_Implementation()
{
	SessionInterface->DestroySession(FName(*MyRoomName));
}

void ULSGameInstance::OnMyExitRoomComplete(FName SessionName, bool bWasSuccesful)
{
	// 플레이어는 LobbyMap으로 여행을 떠나고싶다.
	auto pc = GetWorld()->GetFirstPlayerController();
	FString url = TEXT("/Game/Map/LobbyMap");
	pc->ClientTravel(url, TRAVEL_Absolute);
}

FString ULSGameInstance::StringBase64Encode(const FString& str)
{
	// Set 할 때 : FString -> UTF8 (std::string) -> TArray<uint8> -> base64 로 Encode
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData = TArray<uint8>((uint8*)(utf8String.c_str()), utf8String.length());
	return FBase64::Encode(arrayData);
}

FString ULSGameInstance::StringBase64Decode(const FString& str)
{
	// Get 할 때 : base64 로 Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode(str, arrayData);
	std::string ut8String((char*)(arrayData.GetData()), arrayData.Num());
	return UTF8_TO_TCHAR(ut8String.c_str());
}
