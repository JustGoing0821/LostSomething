// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Interfaces/OnlineSessionInterface.h>
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddRoomInfoDelegate, const FRoomInfo&, RoomInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindingRoomsDelegate, bool, bActive);


UCLASS()
class LOSTSOMETHING_API ULSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//=============================================
	//     PROPERTIES & VARIABLES
	//=============================================
	IOnlineSessionPtr SessionInterface;

	FString NickName = TEXT("MJMC");
	FString MyRoomName;

	//방 검색 구조체
	TSharedPtr<FOnlineSessionSearch> RoomSearch;

	//=============================================
	//     FUCTIONS
	//=============================================

	// 방검색 후 방생성에 관련 델리게이트
	FAddRoomInfoDelegate OnAddRoomInfoDelegate;
	// 방검색에 타이밍에 관련 델리게이트
	FFindingRoomsDelegate OnFindingRoomsDelegate;
	
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
	
};
