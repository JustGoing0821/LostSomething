// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//=============================================
	//     PROPERTIES & VARIABLES
	//=============================================
	UPROPERTY()
	class ULSGameInstance* GI;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcherLobby;

	// -------------------------------------------Main Menu

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_GoCreateRoom;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_GoFindRoom;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class  UEditableText* edit_NickName;

	//--------------------------------------------Create Room

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class  UEditableText* edit_RoomName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_DoCreateRoom;

	//UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	//class UButton* btn_DoVRCreateRoom;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_GoMenuFromCreateRoom;

	//---------------------------------------------Find Room
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_GoMenuFromFindRoom;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UScrollBox* scroll_RoomList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class URoomInfoWidget> RoomInfoFactory;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_DoFindRoomList;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* txt_FindingRooms;

	//=============================================
	//     FUCTIONS
	//=============================================
	virtual void NativeConstruct() override;

	void SwitchPanel(int32 index);

	// 버튼을 누르면 UNetGameInstance::CreateRoom 를 호출하고싶다.
	UFUNCTION(BlueprintCallable)
	void OnMyClicked_doCreateRoom();

	//UFUNCTION(BlueprintCallable)
	//void OnMyClicked_doCreateVRRoom();

	UFUNCTION(BlueprintCallable)
	void OnMyGoMenu();

	UFUNCTION(BlueprintCallable)
	void OnMyGoCreateRoom();

	UFUNCTION(BlueprintCallable)
	void OnMyGoFindRoom();

	UFUNCTION(BlueprintCallable)
	void OnMyDoFindRoomList();

	UFUNCTION(BlueprintCallable)
	void AddRoomInfoWidget(const struct FRoomInfo& roomInfo);

	UFUNCTION(BlueprintCallable)
	void SetFindActive(bool bActive);
	
};
