// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include <string>
#include <Game/LobbyGameMode.h>


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
	Setting.bIsLANMatch = false;
	//Setting.bIsLANMatch = SubSys->GetSubsystemName().IsEqual("NULL");
	//Setting.bIsLANMatch = true;

	Setting.bShouldAdvertise = true;
	Setting.bUsesPresence = true;
	Setting.bAllowJoinInProgress = true;
	Setting.bAllowJoinViaPresence = true;
	Setting.NumPublicConnections = 2;

	// Base64로 통일
	FString EncodedRoomName = StringBase64Encode(RoomName);
	FString EncodedHostName = StringBase64Encode(NickName);

	// 디버깅 로그 추가
	UE_LOG(LogTemp, Warning, TEXT("Original RoomName: %s"), *RoomName);
	UE_LOG(LogTemp, Warning, TEXT("Encoded RoomName: %s"), *EncodedRoomName);

	// 즉시 디코딩해서 검증
	FString DecodedTest = StringBase64Decode(EncodedRoomName);
	UE_LOG(LogTemp, Warning, TEXT("Decode Test: %s"), *DecodedTest);

	// 게임 식별을 위한 고유 키 추가
	Setting.Set(TEXT("game_id"), FString("lostSomething_v1"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("room_name"), EncodedRoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("host_name"), EncodedHostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	Setting.Set(TEXT("player_count"), FString::FromInt(1), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("slot_count"), FString::FromInt(3), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Setting.Set(TEXT("password_required"), FString("false"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(*NetID, FName("MySession"), Setting);
}

void ULSGameInstance::OnMyCreateRoomComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMyCreateRoomComplete!!! sessionName : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);

	if (bWasSuccessful)
	{
		MyRoomName = SessionName.ToString();

		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Error, TEXT("GetWorld() is null"));
			return;
		}

		// 서버인지 체크
		if (World->GetAuthGameMode() == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Not server: GameMode is null"));
			return;
		}

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle,
			[World]() {
				UE_LOG(LogTemp, Warning, TEXT("ServerTravel start"));
				World->ServerTravel(TEXT("/Game/Map/ChooseMap?listen"));
			},
			3.0f,
			false);

		UE_LOG(LogTemp, Warning, TEXT("OnMyCreateRoomComplete: timer success"));
	}
}

void ULSGameInstance::FindOtherRooms()
{
	UE_LOG(LogTemp, Warning, TEXT("FindOtherRooms Begin"));
	// 1. FOnlineSessionSearch객체를 생성
	RoomSearch = MakeShareable(new FOnlineSessionSearch());
	// 2. 세션 검색 조건 설정
	RoomSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 게임 식별자로 필터링 추가
	RoomSearch->QuerySettings.Set(FName(TEXT("game_id")), FString("lostSomething_v1"), EOnlineComparisonOp::Equals);

	// 3. 최대 검색 갯수를 정하고싶다.
	RoomSearch->MaxSearchResults = 10;
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
		if (pc)
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

	int32 ValidRoomCount = 0;

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

		// 게임 ID 확인 (추가 필터링)
		FString gameId;
		bool hasGameId = Settings.Get(FName(TEXT("game_id")), gameId);

		if (!hasGameId || gameId != "lostSomething_v1")
		{
			UE_LOG(LogTemp, Warning, TEXT("Different game session - GameId: %s"), hasGameId ? *gameId : TEXT("None"));
			continue;
		}

		// 우리가 설정한 키들만 체크
		FString roomName, hostName;
		bool hasRoomName = Settings.Get(FName(TEXT("room_name")), roomName);
		bool hasHostName = Settings.Get(FName(TEXT("host_name")), hostName);

		UE_LOG(LogTemp, Warning, TEXT("hasRoomName: %d, hasHostName: %d"), hasRoomName, hasHostName);
		UE_LOG(LogTemp, Warning, TEXT("Raw roomName: %s"), hasRoomName ? *roomName : TEXT("NULL"));
		UE_LOG(LogTemp, Warning, TEXT("Raw hostName: %s"), hasHostName ? *hostName : TEXT("NULL"));

		if (hasRoomName && hasHostName)
		{
			// Base64 디코딩으로 변경
			FString DecodedRoomName = StringBase64Decode(roomName);
			FString DecodedHostName = StringBase64Decode(hostName);

			UE_LOG(LogTemp, Warning, TEXT("encoded room name: %s"), *roomName);
			UE_LOG(LogTemp, Warning, TEXT("decoded room name: %s"), *DecodedRoomName);
			UE_LOG(LogTemp, Warning, TEXT("encoded host name: %s"), *hostName);
			UE_LOG(LogTemp, Warning, TEXT("decoded host name: %s"), *DecodedHostName);

			// 디코딩 검증
			if (DecodedRoomName.IsEmpty() && !roomName.IsEmpty())
			{
				UE_LOG(LogTemp, Error, TEXT("Base64 decode failed for room name! Using raw value."));
				DecodedRoomName = roomName;
			}

			if (DecodedHostName.IsEmpty() && !hostName.IsEmpty())
			{
				UE_LOG(LogTemp, Error, TEXT("Base64 decode failed for host name! Using raw value."));
				DecodedHostName = hostName;
			}

			// UI 델리게이트 호출
			if (OnAddRoomInfoDelegate.IsBound())
			{
				FRoomInfo roomInfo;
				roomInfo.Index = ValidRoomCount;  // 유효한 방만 카운트
				roomInfo.RoomName = DecodedRoomName;
				roomInfo.HostName = DecodedHostName;
				roomInfo.PlayerCount = TEXT("1");
				roomInfo.PingMS = FString::FromInt(SearchResult.PingInMs);

				UE_LOG(LogTemp, Warning, TEXT("AddRoomInfoWidget : begin"));
				OnAddRoomInfoDelegate.Broadcast(roomInfo);
				UE_LOG(LogTemp, Warning, TEXT("AddRoomInfoWidget : end"));
				UE_LOG(LogTemp, Warning, TEXT("OnAddRoomInfoDelegate.Broadcast(roomInfo)"));

				ValidRoomCount++;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing required keys - hasRoomName: %d, hasHostName: %d"), hasRoomName, hasHostName);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Total valid rooms found: %d"), ValidRoomCount);
}

void ULSGameInstance::JoinRoom(int32 index)
{
	// 유효한 방들만 필터링했으므로, 실제 SearchResults 인덱스와 다를 수 있음
	int32 ValidRoomIndex = 0;
	int32 ActualIndex = -1;

	for (int32 i = 0; i < RoomSearch->SearchResults.Num(); i++)
	{
		auto SearchResult = RoomSearch->SearchResults[i];

		if (!SearchResult.IsValid())
			continue;

		const FOnlineSessionSettings& Settings = SearchResult.Session.SessionSettings;

		// 게임 ID 확인
		FString gameId;
		bool hasGameId = Settings.Get(FName(TEXT("game_id")), gameId);
		if (!hasGameId || gameId != "lostSomething_v1")
			continue;

		// room_name과 host_name 확인
		FString roomName, hostName;
		bool hasRoomName = Settings.Get(FName(TEXT("room_name")), roomName);
		bool hasHostName = Settings.Get(FName(TEXT("host_name")), hostName);

		if (!hasRoomName || !hasHostName)
			continue;

		// 유효한 방을 찾았고, 원하는 인덱스인지 확인
		if (ValidRoomIndex == index)
		{
			ActualIndex = i;
			break;
		}
		ValidRoomIndex++;
	}

	if (ActualIndex == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("JoinRoom: Invalid room index %d"), index);
		return;
	}

	auto r = RoomSearch->SearchResults[ActualIndex];
	FString sessionName;
	r.Session.SessionSettings.Get(TEXT("room_name"), sessionName);

	UE_LOG(LogTemp, Warning, TEXT("Joining room with encoded name: %s"), *sessionName);
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
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData;
	arrayData.Append(reinterpret_cast<const uint8*>(utf8String.data()), utf8String.size());

	return FBase64::Encode(arrayData);
}

FString ULSGameInstance::StringBase64Decode(const FString& str)
{
	TArray<uint8> arrayData;
	if (!FBase64::Decode(str, arrayData))
		return FString();

	std::string utf8String(reinterpret_cast<char*>(arrayData.GetData()), arrayData.Num());
	return UTF8_TO_TCHAR(utf8String.c_str());
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