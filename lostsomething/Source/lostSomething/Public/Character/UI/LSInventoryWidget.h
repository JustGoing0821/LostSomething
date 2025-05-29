// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Item/LSItemDataBase.h"
#include "LSInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct();

	void RefreshInventory(const TArray<FItemData>& Inventoroy);

	void OnOffPickupText(ESlateVisibility onoff);

private:

	UPROPERTY(meta = (BindWidget))
	class UListView* ItemList;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PickupText;
};
