// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Interfaces/OnlineSessionInterface.h>
#include "Game/LSNetworkPosition.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Character/Item/LSItemStructures.h"
#include "Game/LevelType.h"
#include "LSGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 Index;
	UPROPERTY(EditDefaultsOnly)
	FString RoomName;
	UPROPERTY(EditDefaultsOnly)
	FString HostName;
	UPROPERTY(EditDefaultsOnly)
	FString PlayerCount;
	UPROPERTY(EditDefaultsOnly)
	FString PingMS;

	FORCEINLINE void PrintLog() const
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomName : %s, HostName:%s, PlayerCount : %s, Ping : %s"), *RoomName, *HostName, *PlayerCount, *PingMS);
	}
};



//=============================================
//     아이템
//=============================================
USTRUCT(BlueprintType)
struct FLSInventorySnapshot
{
	GENERATED_BODY()

public:
	// 슬롯 (어떤 아이템 들어있는지)
	UPROPERTY()
	TArray<FItemDetails> ItemInfos;

	// 현재 선택된 슬롯
	UPROPERTY()
	int32 SelectedSlot = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddRoomInfoDelegate, const FRoomInfo&, RoomInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindingRoomsDelegate, bool, bActive);


UCLASS()
class LOSTSOMETHING_API ULSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	ULSGameInstance();

	//=============================================
	//     PROPERTIES & VARIABLES
	//=============================================
	IOnlineSessionPtr SessionInterface;

	FString NickName = TEXT("MJMC");
	FString MyRoomName;
	bool isVR = false;

	//방 검색 구조체
	TSharedPtr<FOnlineSessionSearch> RoomSearch;

	ELevelType ChooseLevelName;

	TMap<ELevelType, FString> LevelUrlMap;

	UPROPERTY(BlueprintReadWrite)
	FString ServerNickName;

	UPROPERTY(BlueprintReadWrite)
	FString ClientNickName;

	//=============================================
	//     FUCTIONS
	//=============================================

	// 방검색 후 방생성에 관련 델리게이트
	FAddRoomInfoDelegate OnAddRoomInfoDelegate;
	// 방검색에 타이밍에 관련 델리게이트
	FFindingRoomsDelegate OnFindingRoomsDelegate;

	//static const FName ROOM_NAME_KEY(TEXT("CUSTOM_ROOM_NAME"));
//	static const FName HOST_NAME_KEY(TEXT("CUSTOM_HOST_NAME"));
	
	virtual void Init() override;

	//세션 생성 요청 함수
	void CreateRoom(FString RoomName);

	//세션 생성 응답 함수
	void OnMyCreateRoomComplete(FName SessionName, bool bWasSuccessful);

	//세션 검색 요청
	void FindOtherRooms();

	//세션 검색 응답
	void OnMyJoinRoomComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result);

	void OnMyFindOtherRoomsComplete(bool bWasSuccesful);

	//방 입장 요청
	void JoinRoom(int32 index);

	//방 퇴장 요청
	void ExitRoom();

	bool IsInRoom();

	UFUNCTION(Server, Reliable)
	void ServerExitRoom();

	UFUNCTION(NetMulticast, Reliable)
	void MultiExitRoom();

	//로비로 가는 함수
	UFUNCTION()
	void OnMyExitRoomComplete(FName SessionName, bool bWasSuccesful);

	// 글자 바꿔주는 함수
	FString StringBase64Encode(const FString& str);
	FString StringBase64Decode(const FString& str);


	//CurrentQuestSection
public:
	FORCEINLINE const int32 GetCurrentQuestIndex() { return CurrentQuestIndex; }
	FORCEINLINE void SetCurrentQuestIndex(const int32 InQuestIndex) { CurrentQuestIndex = InQuestIndex; }

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Quest)
	int32 CurrentQuestIndex = 0;
	

	//CharacterChoiceSection
public:
	void SetCharacterChoices(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice);
	FORCEINLINE const ELSCharacterChoice GetServerCharacterChoice() { return CharacterChoices[ELSNetworkPosition::Server]; }
	FORCEINLINE const ELSCharacterChoice GetClientCharacterChoice() { return CharacterChoices[ELSNetworkPosition::Client]; }

protected:
	TMap<ELSNetworkPosition, ELSCharacterChoice> CharacterChoices;

	//LevelChoose
public:
	FString GetChooseLevelUrl() const
	{
		const FString* FoundUrl = LevelUrlMap.Find(ChooseLevelName);
		if (FoundUrl)
		{
			return *FoundUrl;
		}

		UE_LOG(LogTemp, Warning, TEXT("LevelType not found in LevelUrlMap!"));
		return TEXT("/Game/Maps/LobbyMap?listen"); // 기본 맵
	}
	FORCEINLINE void SetChooseLevel(ELevelType ChoiceLevel) { ChooseLevelName = ChoiceLevel; }

	//items
public:
	//인벤토리 저장
	void SaveInventory(ELSNetworkPosition Position,const TArray<FItemDetails>& Items, int32 InSelectedSlot);

	// 인벤토리 불러오는 함수
	bool LoadInventory(ELSNetworkPosition Position, TArray<FItemDetails>& OutItems, int32& OutSelectedSlot ) const;


protected:
	//플레이어별 인벤토리 (서버,클라)
	UPROPERTY()
	TMap<ELSNetworkPosition, FLSInventorySnapshot> SavedInventories;

};


