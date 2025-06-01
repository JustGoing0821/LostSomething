// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSInventoryEntry.h"
#include "Character/Item/LSItemDataBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Character/Item/Item.h"
#include "Character/Players/LSPlayer.h"
#include "Character/UI/LSInventoryWidget.h"

void ULSInventoryEntry::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("ULSInventoryEntry::NativeConstruct() called"));

	if (!DropButton)
	{
		UE_LOG(LogTemp, Error, TEXT("DropButton is nullptr in NativeConstruct"));
	}
	else
	{
		DropButton->OnClicked.Clear();
		DropButton->OnClicked.AddDynamic(this, &ULSInventoryEntry::DropItem);
	}
}

void ULSInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UItemUIObject* UIItem = Cast<UItemUIObject>(ListItemObject);
	ItemName->SetText(FText::FromString(UIItem->ItemData.ItemName.ToString()));

	//DropButton->OnClicked.Clear();
	//DropButton->OnClicked.AddDynamic(this, &ULSInventoryEntry::DropItem);
}

void ULSInventoryEntry::DropItem()
{
	UE_LOG(LogTemp, Warning, TEXT("ULSInventoryEntry::DropItem() called"));

	APlayerController* Controller = GetOwningPlayer();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("DropItem() failed: GetOwningPlayer() returned nullptr"));
		return;
	}

	ALSPlayer* PlayerCharacter = Cast<ALSPlayer>(Controller->GetPawn());
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("DropItem() failed: Controller->GetPawn() is nullptr or not ALSPlayer"));
		return;
	}

	UItemUIObject* ItemObject = GetListItem<UItemUIObject>();
	if (!ItemObject)
	{
		UE_LOG(LogTemp, Error, TEXT("DropItem() failed: GetListItem returned nullptr"));
		return;
	}

	PlayerCharacter->DropItem(ItemObject->ItemData);
}

void ULSInventoryEntry::ClearButton()
{
	if (DropButton && DropButton->OnClicked.IsBound())
	{
		DropButton->OnClicked.RemoveAll(this); // 명시적으로 현재 클래스만 제거
	}
}
