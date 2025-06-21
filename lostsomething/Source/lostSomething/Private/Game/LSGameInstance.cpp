// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include <string>


ULSGameInstance::ULSGameInstance()
{
	CharacterChoices.Add(ELSNetworkPosition::Server, ELSCharacterChoice::None);
	CharacterChoices.Add(ELSNetworkPosition::Client, ELSCharacterChoice::None);
}

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

	Setting.bIsDedicated = false;

	auto SubSys = IOnlineSubsystem::Get();
	Setting.bIsLANMatch = SubSys->GetSubsystemName().IsEqual("NULL");

	Setting.bShouldAdvertise = true;
	Setting.bUsesPresence = true;
	Setting.bAllowJoinInProgress = true;
	Setting.bAllowJoinViaPresence = true;
	Setting.NumPublicConnections = 2;

	// ? 값은 반드시 UTF-8 또는 FString로 저장 (Base64 제거)
	FString EncodedRoomName = StringBase64Encode(RoomName);  // 변경!
	FString EncodedHostName = StringBase64Encode(NickName);  // 변경!

	Setting.Set(TEXT("room_name"), EncodedRoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("host_name"), EncodedHostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 숫자 값도 반드시 FString으로 변환
	Setting.Set(TEXT("player_count"), FString::FromInt(1), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("slot_count"), FString::FromInt(3), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("password_required"), FString("false"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(*netID, FName("MySession"), Setting);
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
	UE_LOG(LogTemp, Warning, TEXT("FindOtherRooms Begin"));
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
		UE_LOG(LogTemp, Warning, TEXT("OnFindingRoomsDelegate.Broadcast"));
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
	UE_LOG(LogTemp, Warning, TEXT("OnMyFindOtherRoomsComplete CALLED!!!"));
	UE_LOG(LogTemp, Warning, TEXT("search success: %d"), bWasSuccesful);
	UE_LOG(LogTemp, Warning, TEXT("find room numbers: %d"), RoomSearch->SearchResults.Num());

	for (int32 i = 0; i < RoomSearch->SearchResults.Num(); i++)
	{
		auto SearchResult = RoomSearch->SearchResults[i];

		UE_LOG(LogTemp, Warning, TEXT("=== room [%d] information ==="), i);

		if (!SearchResult.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("INVALID SESSION"));
			continue;
		}

		const FOnlineSessionSettings& Settings = SearchResult.Session.SessionSettings;

		// 우리가 설정한 키들만 체크
		FString roomName, hostName;
		bool hasRoomName = Settings.Get(FName(TEXT("room_name")), roomName);
		bool hasHostName = Settings.Get(FName(TEXT("host_name")), hostName);

		if (hasRoomName && hasHostName)
		{
			UE_LOG(LogTemp, Warning, TEXT("room name: %s"), *StringBase64Decode(roomName));
			UE_LOG(LogTemp, Warning, TEXT("host: %s"), *StringBase64Decode(hostName));

			// UI 델리게이트 호출
			if (OnAddRoomInfoDelegate.IsBound())
			{
				FRoomInfo roomInfo;
				roomInfo.Index = i;
				roomInfo.RoomName = StringBase64Decode(roomName);
				roomInfo.HostName = StringBase64Decode(hostName);
				roomInfo.PlayerCount = TEXT("1");
				roomInfo.PingMS = FString::FromInt(SearchResult.PingInMs);

				OnAddRoomInfoDelegate.Broadcast(roomInfo);
				UE_LOG(LogTemp, Warning, TEXT("OnAddRoomInfoDelegate.Broadcast(roomInfo)"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no room"));
		}
	}
}

void ULSGameInstance::JoinRoom(int32 index)
{
	auto r = RoomSearch->SearchResults[index];
	FString sessionName;
	r.Session.SessionSettings.Get(TEXT("room_name"), sessionName);
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

void ULSGameInstance::SetCharacterChoices(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{
	//UE_LOG(LogTemp, Log, TEXT("%s"), TEXT("SetCharacterChoices Begin"));
	if (CharacterChoices.Num() == 2)
	{
		CharacterChoices[ELSNetworkPosition::Server] = ServerChoice;
		CharacterChoices[ELSNetworkPosition::Client] = ClientChoice;
	}
}
