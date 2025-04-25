// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API URoomInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//=============================================
	//     PROPERTIES & VARIABLES
	//=============================================
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* txt_RoomName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* txt_HostName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* txt_Ping;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_Join;

	int32 RoomIndex;

	//=============================================
	//     FUCTIONS
	//=============================================
	virtual void NativeConstruct() override;

	void SetInfo(const struct FRoomInfo& info);

	UFUNCTION()
	void OnMyJoinRoom();
	
};
