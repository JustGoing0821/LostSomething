// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSInventoryWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"

void ULSInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void ULSInventoryWidget::RefreshInventory(const TArray<FItemData>& Inventory)
{
	UE_LOG(LogTemp, Warning, TEXT("RefreshInventory() called. Inventory count: %d"), Inventory.Num());

	// 아이템 리스트 초기화
	if (ItemList)
	{
		ItemList->ClearListItems();
		UE_LOG(LogTemp, Warning, TEXT("ItemList cleared."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemList is nullptr!"));
		return;
	}

	// 인벤토리 순회하며 추가
	for (const FItemData& ItemData : Inventory)
	{
		UItemUIObject* UIItem = NewObject<UItemUIObject>(this);
		if (!UIItem)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create UIItem object!"));
			continue;
		}

		UIItem->ItemData = ItemData;

		ItemList->AddItem(UIItem);

		UE_LOG(LogTemp, Warning, TEXT("Added Item: %s (Class: %s)"),
			*ItemData.ItemName.ToString(),
			*GetNameSafe(ItemData.Class));
	}
}


void ULSInventoryWidget::OnOffPickupText(ESlateVisibility onoff)
{

	PickupText->SetVisibility(onoff);
}
